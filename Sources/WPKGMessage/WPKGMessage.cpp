// WPKGMessage.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WPKGMessage.h"
#include "..\components\filemap.h"
#include "WPKGMessageDlg.h"

//#include "..\components\Report.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL bWorking = TRUE;
BOOL bConnected = FALSE;
CEvent eTestServiceStatus;

static CFileMap startTimeInfo;


UINT WaitForDone(LPVOID)
{
	//CReport rep;
		
	//rep.OpenReport("c:\\wpkglogon.txt");
	
		

	DWORD dwTimeOut = 60; // in seconds
	


	HKEY phkResult;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Notify\\WPKGLogon",
		0,
		KEY_READ,
		&phkResult );

	DWORD cbData = 4;
	DWORD Type;
	
	// in registry timeout is in seconds
	DWORD OK = RegQueryValueEx( phkResult,
		"MaxWait", 0, &Type, (BYTE*)&dwTimeOut, &cbData );

	RegCloseKey(phkResult); 


	// logon delay must be longer then wpkgmessage wait
	if(dwTimeOut >= 90)
		dwTimeOut -= 30;
	
	
	
	CTime date;
	CTime current = date = CTime::GetCurrentTime();
	CTimeSpan ts;

	//rep.AddInfo("Logon timeout = %u s",dwTimeOut);

	dwTimeOut *= 1000;
	DWORD dwMaxTick = GetTickCount();
	dwMaxTick += dwTimeOut;
	//rep.AddInfo("dwMaxTick = %u ms",dwMaxTick);


	
	DWORD dwTick = 0;

	while(bWorking && dwTick<dwMaxTick)
	{
		try
		{
			startTimeInfo.OpenSharedMem();
			date = startTimeInfo.ReadStartDate();
			startTimeInfo.LogonDelayWorking();
			//rep.AddInfo("Open shared memory = %s",date.Format("%Y-%m-%d %H:%M:%S"));
			bConnected = TRUE;
			break;
		}
		catch(CException* e)
		{
			//rep.AddInfo("Cant open shared memory, dwTick = %u",dwTick);
			
			e->Delete();
			Sleep(5000);
			dwTick = GetTickCount();
			eTestServiceStatus.SetEvent();
			continue;
		}
	}
	
	ts = current - date;
	
	LONGLONG timeElapsed = ts.GetTotalSeconds();

	//rep.AddInfo("timeElapsed = %d s",timeElapsed);


	if(timeElapsed < ((dwMaxTick - dwTick)/1000) )
	{
		eTestServiceStatus.SetEvent();

		CString str;

		while(bWorking && startTimeInfo.IsWorking(str,dwMaxTick))
		{
			//rep.AddInfo("IsWorking tick = %u",GetTickCount());
			CWnd* pWnd = AfxGetApp()->m_pMainWnd;
			
			if(pWnd && ::IsWindow(pWnd->GetSafeHwnd()))
			{
				pWnd->SendMessage(WM_USER+101,0,(LPARAM)str.GetBuffer());

			}
		}

		//for(;;)
		//{
		//	BOOL bResult = startTimeInfo.IsWorking(str,dwMaxTick);

		//	rep.AddInfo("IsWorking tick = %u",GetTickCount());
		//	CWnd* pWnd = AfxGetApp()->m_pMainWnd;

		//	if(pWnd && ::IsWindow(pWnd->GetSafeHwnd()))
		//	{
		//		pWnd->SendMessage(WM_USER+101,0,(LPARAM)str.GetBuffer());

		//	}
		//	if(!bResult)
		//		break;
		//}


	}

	//rep.AddInfo("Waiting done");
	bWorking = FALSE;
	eTestServiceStatus.SetEvent();
	return 0;
}


BOOL IsWorking()
{
	return bWorking;
}

BOOL IsWorkingDone()
{
	return (!bWorking && bConnected);
}


void WorkingDone()
{
	bWorking = FALSE;
	startTimeInfo.AbortWorkingMonitor();
}



// CWPKGMessageApp

BEGIN_MESSAGE_MAP(CWPKGMessageApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWPKGMessageApp construction

CWPKGMessageApp::CWPKGMessageApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CWPKGMessageApp object

CWPKGMessageApp theApp;


// CWPKGMessageApp initialization

BOOL CWPKGMessageApp::InitInstance()
{
	CWinApp::InitInstance();
	
	CWinThread* pThr = AfxBeginThread(WaitForDone,NULL);
		
	CSingleLock lck(&eTestServiceStatus);
	lck.Lock();

	if(!IsWorkingDone())
	{
		CWPKGMessageDlg dlg;
		m_pMainWnd = &dlg;
		dlg.DoModal();
	}

	lck.Unlock();
	eTestServiceStatus.Unlock();

	WaitForSingleObject(pThr->m_hThread,INFINITE);
	return FALSE;
}
