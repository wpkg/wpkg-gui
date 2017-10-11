#include "stdafx.h"
#include "WpkgWorker.h"

CWpkgWorker::CWpkgWorker(void)
{
	m_bRestartSystem = FALSE;
}

CWpkgWorker::~CWpkgWorker(void)
{
}

void CWpkgWorker::Cleanup()
{
	CEventLog::AddMessageToLog("Working done. Perform cleanup."); 
	m_wokerProgressInfo.WriteStartDate(CTime(0));
	m_wokerProgressInfo.WaitForLogonDelay();
	CEventLog::AddMessageToLog("Cleanup done."); 
}


void CWpkgWorker::WpkgClientAction()
{

	HANDLE hToken = NULL;

	CString netPath = secret.m_strScriptFile;
	netPath.Remove('\"');
	CString actionPath = secret.m_strScriptFile;
	actionPath.Remove('\"');
	actionPath.Insert(0,'\"');
	actionPath+="\"";


	m_wokerProgressInfo.AllowAdminAccesSa();
	CEventLog::AddMessageToLog("Before create shared memory: successfuly done."); 
	m_wokerProgressInfo.CreateSharedMem();
	CEventLog::AddMessageToLog("Create shared memory: successfuly done."); 
	m_wokerProgressInfo.WriteStartDate(CTime::GetCurrentTime());

		// Switch to required security context
	security.LogonUser(secret.m_strScriptExecUser.GetBuffer(),
		secret.m_strScriptExecPassword.GetBuffer(),hToken);

	// add message to event log
	CEventLog::AddMessageToLog("Set script security context: successfuly done."); 



	// new feature laptop mode added
	if(secret.m_bLaptopMode)
	{
		CEventLog::AddMessageToLog("Offline mode enabled: successfuly done."); 
		switch(secret.m_iServerTestingMethod)
		{
		case 0:
			{
				CServerPing serverPing;
				CEventLog::AddMessageToLog("Offline mode: server connecting method selected."); 

				BOOL serverConnected = serverPing.WaitForConnect(secret.m_strServerIP,secret.m_dwServerPingTimeout);

				if(!serverConnected)
					CExceptionEx::ThrowError("Server connecting: failed.");
				else
					CEventLog::AddMessageToLog("Server connecting: successfuly done."); 
			}
			break;
		case 1:
			{
				CEventLog::AddMessageToLog("Offline mode: custom connecting script method selected."); 
				if(PathIsNetworkPath(netPath) && !secret.m_bNetUseMachineAccount)
				{
					connection.AddConnection(secret.m_ServerPingScriptFile,
						secret.m_strScriptConnUser,secret.m_strScriptConnPassword);
					CEventLog::AddMessageToLog("Network resource for custom script required: successfuly connected"); 
				}

				DWORD exitCode = process.CreateProcess(hToken, secret.m_ServerPingScriptFile.GetBuffer(),secret.m_bShowGUI, secret.m_dwPriority, secret.m_dwServerPingScriptTimeout*1000);
				connection.Disconnect(secret.m_ServerPingScriptFile);
				CEventLog::AddMessageToLog("Network resource for custom script was required: successfuly disconnected"); 

				if(exitCode==0)
					CEventLog::AddMessageToLog("Custom connecting script: successfuly done."); 
				else
					CExceptionEx::ThrowError("Custom connecting script: failed."); 


			}
			break;
		}

	}


	// connect to the remote resource if required
	

	if(PathIsNetworkPath(netPath) && !secret.m_bNetUseMachineAccount)
	{
		CEventLog::AddMessageToLog("Network resource required"); 
		connection.AddConnection(netPath,
			secret.m_strScriptConnUser,secret.m_strScriptConnPassword);

		CEventLog::AddMessageToLog("Network resource: successfuly connected"); 
	}



	CString command("cscript.exe ");

	command += actionPath;
	command += " ";
	command += secret.m_strScriptParameters;

	BOOL OK;
	DWORD exitCode = 0;

	for(int i=0; i<secret.m_strVarArray.GetCount(); i+=2)
	{
		OK = SetEnvironmentVariable(secret.m_strVarArray.GetAt(i), secret.m_strVarArray.GetAt(i+1));
	}

	if(!secret.m_strPreAction.IsEmpty())
	{
		security.AddDesktopPermission("winsta0","default",hToken,secret.m_bShowGUI);
		exitCode = process.CreateProcess(hToken, secret.m_strPreAction.GetBuffer(),secret.m_bShowGUI, secret.m_dwPriority);
		CString str;
		str.Format("Script pre action execution: failure. Exit code: %u",exitCode);
		if(exitCode==0)
			CEventLog::AddMessageToLog("Script pre action execution: successfuly done");
		else
			CExceptionEx::ThrowError(str);
	}

	progressMessage.SetSharedMemory(&m_wokerProgressInfo);
	progressMessage.CreatePipe();
	process.m_eventSender.AddObserver((CEventHandler*)&progressMessage);

	// run cscript and wait for termination
	CEventLog::AddMessageToLog("Starting script action execution");
	exitCode = process.CreateProcess(hToken, command.GetBuffer(),secret.m_bShowGUI, secret.m_dwPriority, INFINITE);
	CEventLog::AddMessageToLog("Script action execution done.");


	process.m_eventSender.RemoveAll();

	switch(exitCode)
	{
	case 0:
		process.m_dwRestartCount = 0;
		process.WriteRestartInfo();

		CEventLog::AddMessageToLog("Script execution: successfuly done"); 
		break;
	case 3010:
		{

			process.ReadRestartInfo();

			if(process.m_dwRestartCount <=5)
			{
				m_bRestartSystem = TRUE;
			}
			else
			{
				CExceptionEx::ThrowError("Script execution required restart but restart count exceed 5 times one after the other. Restart canceled.");

			}

		}
		break;
	default:
		process.m_dwRestartCount = 0;
		process.WriteRestartInfo();
		CString str;
		str.Format("Script execution: failure. Exit code: %u",exitCode);

		CExceptionEx::ThrowError(str);

	}


	if(!secret.m_strPostAction.IsEmpty())
	{
		security.AddDesktopPermission("winsta0","default",hToken,secret.m_bShowGUI);
		CEventLog::AddMessageToLog("Starting script post action execution");
		exitCode = process.CreateProcess(hToken, secret.m_strPostAction.GetBuffer(),secret.m_bShowGUI,secret.m_dwPriority);
		CString str;
		str.Format("Script post action execution: failure. Exit code: %u",exitCode);
		if(exitCode==0)
			CEventLog::AddMessageToLog("Script post action execution: successfuly done");
		else
			CExceptionEx::ThrowError(str);

	}

	// disconnect from network resource
	if(PathIsNetworkPath(netPath) && !secret.m_bNetUseMachineAccount)
	{
		connection.Disconnect(netPath);
		CEventLog::AddMessageToLog("Network resource: successfuly disconnected"); 
	}



	// working done
	m_wokerProgressInfo.WriteStartDate(CTime(0));


	Sleep(3000);



}

void CWpkgWorker::PerformRestart(int iLoggedUsers)
{
	if(m_bRestartSystem == TRUE)
	{
		process.RestartSystem("WPKG Service initiated system reboot",iLoggedUsers);
		process.m_dwRestartCount++;
		process.WriteRestartInfo();
		CEventLog::AddMessageToLog("Script execution required restart: successfuly done"); 
	}
}

BOOL CWpkgWorker::IsMustStop()
{
	return secret.m_bStopServiceAfterDone;
}

BOOL CWpkgWorker::IsRunOnShutdown()
{
	return secret.m_bRunOnShutdown;
}

void CWpkgWorker::Initialize(void)
{
	::CoInitialize(NULL);
	// Load secret data
	secret.LoadSecret();
	CEventLog::m_PathToLogFile = secret.m_strLogFile;

}
