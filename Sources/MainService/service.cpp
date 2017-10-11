#include "stdafx.h"
#include "service.h"
#include "..\components\security.h"
#include "..\components\EventLog.h"
#include "..\components\WpkgWorker.h"


/// test sens
#include "WTSAPI32.H"



// internal variables
SERVICE_STATUS          ssStatus;       // current status of the service
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwErr = 0;
TCHAR                   szErr[256];

BOOL bDebug = FALSE;

static CWpkgWorker wpkgWorker;

static int iUserLoggedCount = 0;

static HANDLE  hServerEvents[3] = {NULL,NULL,NULL};


//
//  FUNCTION: service_main
//
//  PURPOSE: To perform actual initialization of the service
//
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    This routine performs the service initialization and then calls
//    the user defined ServiceStart() routine to perform majority
//    of the work.
//
void WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	CEventLog::m_strAppName = TEXT(SZSERVICENAME);

	CEventLog::AddMessageToLog("WPKG Service trying start...");


	// register our service control handler:
	sshStatusHandle = RegisterServiceCtrlHandlerEx( TEXT(SZSERVICENAME), (LPHANDLER_FUNCTION_EX)service_ctrlEx, NULL);


	if (!sshStatusHandle)
	{
		DWORD err = GetLastError();
		CEventLog::AddErrorMessageToLog("RegisterServiceCtrlHandlerEx: FAILED");
		goto cleanup;
	}

	CEventLog::AddMessageToLog("WPKG Service->RegisterServiceCtrlHandler: successfuly done");

	// SERVICE_STATUS members that don't change in example
	//
	ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ssStatus.dwServiceSpecificExitCode = 0;

	// report the status to the service control manager.
	//
	if (!ReportStatusToSCMgr(
		SERVICE_START_PENDING, // service state
		NO_ERROR,              // exit code
		3000))                 // wait hint
		goto cleanup;


	CEventLog::AddMessageToLog("WPKG Service->SERVICE_START_PENDING: successfuly done");
	ServiceStart( dwArgc, lpszArgv );

cleanup:

	// try to report the stopped status to the service control manager.
	//
	if (sshStatusHandle)
		(VOID)ReportStatusToSCMgr(
		SERVICE_STOPPED,
		dwErr,
		0);

	return;
}

///////////////////////////////////////////////////////////////////
//
//  The following code is for running the service as a console app
//


//
//  FUNCTION: CmdDebugService(int argc, char ** argv)
//
//  PURPOSE: Runs the service as a console application
//
//  PARAMETERS:
//    argc - number of command line arguments
//    argv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
void CmdDebugService(int argc, char ** argv)
{
	DWORD dwArgc;
	LPTSTR *lpszArgv;

#ifdef UNICODE
	lpszArgv = CommandLineToArgvW(GetCommandLineW(), &(dwArgc) );
#else
	dwArgc   = (DWORD) argc;
	lpszArgv = argv;
#endif

	_tprintf(TEXT("Debugging %s.\n"), TEXT(SZSERVICEDISPLAYNAME));

	SetConsoleCtrlHandler( ControlHandler, TRUE );

	ServiceStart( dwArgc, lpszArgv );
}





//
//  FUNCTION: service_ctrlEx
//
//  PURPOSE: This function is called by the SCM whenever
//           ControlService() is called on this service.
//
//  PARAMETERS:
//    dwCtrlCode - type of control requested
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//


VOID WINAPI service_ctrlEx(DWORD dwCtrlCode, DWORD dwEventType,
						   LPVOID lpEventData,
						   LPVOID lpContext
						   )
{

	// Handle the requested control code.
	//
	switch(dwCtrlCode)
	{
		// Stop the service.
		//
		// SERVICE_STOP_PENDING should be reported before
		// setting the Stop Event - hServerEvents[0] - in
		// ServiceStop().  This avoids a race condition
		// which may result in a 1053 - The Service did not respond...
		// error.

	case SERVICE_CONTROL_SHUTDOWN:
		wpkgWorker.WakeUpProcesses();
		ServiceStop();
		CEventLog::AddMessageToLog("System send shutdown signal");
		return;
	case SERVICE_CONTROL_STOP:
		wpkgWorker.TerminateProcesses();
		ServiceStop();
		CEventLog::AddMessageToLog("System send stop signal");
		return;
	case SERVICE_CONTROL_PRESHUTDOWN:
		wpkgWorker.WakeUpProcesses();
		ServiceStop();
		CEventLog::AddMessageToLog("System send pre-shutdown signal");
		return;

		// Update the service status.
		//

	case SERVICE_CONTROL_INTERROGATE:
		break;

		// only 2000 PRO and higer
		//
		// 0x0000000E
	case SERVICE_CONTROL_SESSIONCHANGE:
		{

			switch(dwEventType)
			{
			case WTS_SESSION_LOGOFF:
				if(iUserLoggedCount>0)
					iUserLoggedCount--;
				break;
			case WTS_SESSION_LOGON:
				iUserLoggedCount++;
				break;
			}

		}
		break;
		// invalid control code
	default:
		break;

	}

	CEventLog::AddMessageToLog("System query service status");
	ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
	//ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 3000);
}



//
//  FUNCTION: ReportStatusToSCMgr()
//
//  PURPOSE: Sets the current status of the service and
//           reports it to the Service Control Manager
//
//  PARAMETERS:
//    dwCurrentState - the state of the service
//    dwWin32ExitCode - error code to report
//    dwWaitHint - worst case estimate to next checkpoint
//
//  RETURN VALUE:
//    TRUE  - success
//    FALSE - failure
//
//  COMMENTS:
//
BOOL ReportStatusToSCMgr(DWORD dwCurrentState,
						 DWORD dwWin32ExitCode,
						 DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;
	BOOL fResult = TRUE;


	if ( !bDebug ) // when debugging we don't report to the SCM
	{
		if (dwCurrentState == SERVICE_START_PENDING)
			ssStatus.dwControlsAccepted = 0;
		else
		{
			DWORD flag = SERVICE_ACCEPT_SESSIONCHANGE;
			OSVERSIONINFO osi;
			osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			BOOL OK = GetVersionEx( &osi );

			if(OK)
			{
				if(osi.dwMajorVersion<5)
					flag = 0;
				if(osi.dwMajorVersion==5 && osi.dwMinorVersion<1)
					flag = 0;
		
				if(osi.dwMajorVersion>5)
					flag|=SERVICE_ACCEPT_PRESHUTDOWN;
			}
			else
				flag = 0;


			ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_SHUTDOWN|flag;
		}

		ssStatus.dwCurrentState = dwCurrentState;
		ssStatus.dwWin32ExitCode = dwWin32ExitCode;
		ssStatus.dwWaitHint = dwWaitHint;

		if ( ( dwCurrentState == SERVICE_RUNNING ) ||
			( dwCurrentState == SERVICE_STOPPED ) )
			ssStatus.dwCheckPoint = 0;
		else
			ssStatus.dwCheckPoint = dwCheckPoint++;


		// Report the status of the service to the service control manager.
		//
		fResult = SetServiceStatus( sshStatusHandle, &ssStatus);
	}
	return fResult;
}



///////////////////////////////////////////////////////////////////
//
//  The following code handles service installation and removal
//


//
//  FUNCTION: CmdInstallService()
//
//  PURPOSE: Installs the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
void CmdInstallService(char* user, char* password)
{
	SC_HANDLE   schService;
	SC_HANDLE   schSCManager;

	TCHAR szPath[512];

	if ( GetModuleFileName( NULL, szPath, 512 ) == 0 )
	{
		printf(TEXT("Unable to install %s - %s\n"), TEXT(SZSERVICEDISPLAYNAME), GetLastErrorText(szErr, 256));
		return;
	}

	schSCManager = OpenSCManager(
		NULL,                   // machine (NULL == local)
		NULL,                   // database (NULL == default)
		SC_MANAGER_ALL_ACCESS   // access required
		);
	if ( schSCManager )
	{
		schService = CreateService(
			schSCManager,               // SCManager database
			TEXT(SZSERVICENAME),        // name of service
			TEXT(SZSERVICEDISPLAYNAME), // name to display
			SERVICE_ALL_ACCESS,         // desired access
			SERVICE_WIN32_OWN_PROCESS|
			SERVICE_INTERACTIVE_PROCESS,// service type
			SERVICE_AUTO_START,         // start type
			SERVICE_ERROR_NORMAL,       // error control type
			szPath,                     // service's binary
			NULL,                       // no load ordering group
			NULL,                       // no tag identifier
			TEXT(SZDEPENDENCIES),       // dependencies
			user,                       // LocalSystem account
			password);                  // no password

		if ( schService )
		{
			SERVICE_DESCRIPTION desc;
			desc.lpDescription = SZSERVICEDESCRIPTION;
			ChangeServiceConfig2(schService,SERVICE_CONFIG_DESCRIPTION,&desc);
			_tprintf(TEXT("%s installed.\n"), TEXT(SZSERVICEDISPLAYNAME) );
			CloseServiceHandle(schService);
		}
		else
		{
			_tprintf(TEXT("CreateService failed - %s\n"), GetLastErrorText(szErr, 256));
		}

		CloseServiceHandle(schSCManager);
	}
	else
		_tprintf(TEXT("OpenSCManager failed - %s\n"), GetLastErrorText(szErr,256));
}



//
//  FUNCTION: CmdRemoveService()
//
//  PURPOSE: Stops and removes the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
void CmdRemoveService()
{
	SC_HANDLE   schService;
	SC_HANDLE   schSCManager;

	schSCManager = OpenSCManager(
		NULL,                   // machine (NULL == local)
		NULL,                   // database (NULL == default)
		SC_MANAGER_ALL_ACCESS   // access required
		);
	if ( schSCManager )
	{
		schService = OpenService(schSCManager, TEXT(SZSERVICENAME), SERVICE_ALL_ACCESS);

		if (schService)
		{
			// try to stop the service
			if ( ControlService( schService, SERVICE_CONTROL_STOP, &ssStatus ) )
			{
				_tprintf(TEXT("Stopping %s."), TEXT(SZSERVICEDISPLAYNAME));
				Sleep( 1000 );

				while( QueryServiceStatus( schService, &ssStatus ) )
				{
					if ( ssStatus.dwCurrentState == SERVICE_STOP_PENDING )
					{
						_tprintf(TEXT("."));
						Sleep( 1000 );
					}
					else
						break;
				}

				if ( ssStatus.dwCurrentState == SERVICE_STOPPED )
					_tprintf(TEXT("\n%s stopped.\n"), TEXT(SZSERVICEDISPLAYNAME) );
				else
					_tprintf(TEXT("\n%s failed to stop.\n"), TEXT(SZSERVICEDISPLAYNAME) );

			}

			// now remove the service
			if( DeleteService(schService) )
				_tprintf(TEXT("%s removed.\n"), TEXT(SZSERVICEDISPLAYNAME) );
			else
				_tprintf(TEXT("DeleteService failed - %s\n"), GetLastErrorText(szErr,256));


			CloseServiceHandle(schService);
		}
		else
			_tprintf(TEXT("OpenService failed - %s\n"), GetLastErrorText(szErr,256));

		CloseServiceHandle(schSCManager);
	}
	else
		_tprintf(TEXT("OpenSCManager failed - %s\n"), GetLastErrorText(szErr,256));
}




///////////////////////////////////////////////////////////////////
//
//  The following code is for running the service as a console app
//



//
//  FUNCTION: ControlHandler ( DWORD dwCtrlType )
//
//  PURPOSE: Handled console control events
//
//  PARAMETERS:
//    dwCtrlType - type of control event
//
//  RETURN VALUE:
//    True - handled
//    False - unhandled
//
//  COMMENTS:
//
BOOL WINAPI ControlHandler ( DWORD dwCtrlType )
{
	switch( dwCtrlType )
	{
	case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
	case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
		//TODO: block in release version
		ServiceStop();

		return TRUE;
	}
	return FALSE;
}

//
//  FUNCTION: GetLastErrorText
//
//  PURPOSE: copies error message text to string
//
//  PARAMETERS:
//    lpszBuf - destination buffer
//    dwSize - size of buffer
//
//  RETURN VALUE:
//    destination buffer
//
//  COMMENTS:
//
LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize )
{
	DWORD dwRet;
	LPTSTR lpszTemp = NULL;

	dwRet = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
		NULL,
		GetLastError(),
		LANG_NEUTRAL,
		(LPTSTR)&lpszTemp,
		0,
		NULL );

	// supplied buffer is not long enough
	if ( !dwRet || ( (long)dwSize < (long)dwRet+14 ) )
		lpszBuf[0] = TEXT('\0');
	else
	{
		lpszTemp[lstrlen(lpszTemp)-2] = TEXT('\0');  //remove cr and newline character
		_stprintf( lpszBuf, TEXT("%s (0x%x)"), lpszTemp, GetLastError() );
	}

	if ( lpszTemp )
		LocalFree((HLOCAL) lpszTemp );

	return lpszBuf;
}



VOID ServiceStart (DWORD dwArgc, LPTSTR *lpszArgv)
{
	try
	{
		// initialization
		wpkgWorker.Initialize();


		///////////////////////////////////////////////////
		//
		// Service initialization
		//


		// report the status to the service control manager.
		//
		if (!ReportStatusToSCMgr(
			SERVICE_START_PENDING, // service state
			NO_ERROR,              // exit code
			3000))                 // wait hint
		{
			CEventLog::AddErrorMessageToLog("ServiceStart->ReportStatusToSCMgr failed");
			return;
		}

		CSecurity::AllowAdminAccesSa();


		// create the event object. The control handler function signals
		// this event when it receives the "stop" control code.
		//
		hServerEvents[0] = CreateEvent(
			NULL,    // no security attributes
			FALSE,    // manual reset event
			FALSE,   // not-signalled
			NULL);   // no name

		if ( hServerEvents[0] == NULL)
		{
			CEventLog::AddErrorMessageToLog("ServiceStart->hServerEvents[0] == NULL");
			return;
		}


		// manual start event
		hServerEvents[1] = CreateEvent(
			&CSecurity::m_sa,     // access for Admins & system
			FALSE,				// manual reset event
			FALSE,				// not-signalled
			"Global\\START_WPKG_SRVR");   

		if ( hServerEvents[1] == NULL)
		{
			CEventLog::AddErrorMessageToLog("ServiceStart->hServerEvents[1] == NULL");
			return;
		}

		// viewer stop event
		hServerEvents[2] = CreateEvent(
			&CSecurity::m_sa,     // access for Admins &  system
			FALSE,				// manual reset event
			FALSE,				// not-signalled
			"Global\\STOP_WPKG_SRVR");   

		if ( hServerEvents[2] == NULL)
		{
			CEventLog::AddErrorMessageToLog("ServiceStart->hServerEvents[2] == NULL");
			return;
		}


		// Initialise the Application here
		// or upgrade to the professional version to get the full application thread

		// report the status to the service control manager.
		//
		if (!ReportStatusToSCMgr(
			SERVICE_RUNNING,       // service state
			NO_ERROR,              // exit code
			0))                    // wait hint
		{
			CEventLog::AddErrorMessageToLog("ServiceStart->SERVICE_RUNNING: failed");
			return;
		}

		CEventLog::AddMessageToLog("ServiceStart->SERVICE_RUNNING");
		//
		// End of initialization
		//
		////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////
		//
		// Service is now running, perform work until shutdown
		////////////////////////////////////////////////////////

		if(!wpkgWorker.IsRunOnShutdown())
		{
			CEventLog::AddMessageToLog("Starting WPKG on startup");

			wpkgWorker.WpkgClientAction();
			
			if(wpkgWorker.IsMustStop())
				ServiceStop();

			wpkgWorker.PerformRestart(iUserLoggedCount);
		}

	}
	catch(CException* exc)
	{
		char message[1024];
		exc->GetErrorMessage(message,1024);
		CEventLog::AddErrorMessageToLog(message);
		exc->Delete();
		ServiceStop();
		goto cleanup;
	}
	catch(...)
	{
		CEventLog::AddErrorMessageToLog("Unknown error occured");
		ServiceStop();
		goto cleanup;
	}


	while(TRUE)
	{
		DWORD dwWait = WaitForMultipleObjects(3, hServerEvents, FALSE,  INFINITE );
		if ( dwWait == WAIT_OBJECT_0 )       // not overlapped i/o event - error occurred,
		{
			CEventLog::AddMessageToLog("SetEvent STOP (WAIT_OBJECT_0)");
		}
			break;                           // or server stop signaled

		if ( dwWait == WAIT_OBJECT_0+1 )     // manual start event
		{
			try
			{
				
				CEventLog::AddMessageToLog("Starting WPKG on shutdown");
				ServiceStop();

			}
			catch(CException* exc)
			{
				char message[1024];
				exc->GetErrorMessage(message,1024);
				CEventLog::AddErrorMessageToLog(message);
				exc->Delete();
				ServiceStop();
				goto cleanup;
			}
			catch(...)
			{
				CEventLog::AddErrorMessageToLog("Unknown error occured");
				ServiceStop();
				goto cleanup;
			}	

		}

		if ( dwWait == WAIT_OBJECT_0+2 )     // manual stop event
		{
			try
			{
				//...feature for manual stop signal
			}
			catch(CException* e)
			{
				e->Delete();
			}
		}


	}

	// action after shutdown signal
	// not possible on VERSION_NT < 0x600 !
	try
	{
		if(wpkgWorker.IsRunOnShutdown())
			wpkgWorker.WpkgClientAction();

	}
	catch(CException* exc)
	{
		char message[1024];
		exc->GetErrorMessage(message,1024);
		CEventLog::AddErrorMessageToLog(message);
		exc->Delete();
		ServiceStop();
		goto cleanup;
	}
	catch(...)
	{
		CEventLog::AddErrorMessageToLog("Unknown error occured");
		ServiceStop();
		goto cleanup;
	}	



	CEventLog::AddMessageToLog("WPKG Service: successfuly done");


cleanup:

	// working done
	wpkgWorker.Cleanup();


	if (hServerEvents[0])
		CloseHandle(hServerEvents[0]);

	if (hServerEvents[1])
		CloseHandle(hServerEvents[1]);

	if (hServerEvents[2])
		CloseHandle(hServerEvents[2]);

}





//
//  FUNCTION: ServiceStop
//
//  PURPOSE: Stops the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    If a ServiceStop procedure is going to
//    take longer than 3 seconds to execute,
//    it should spawn a thread to execute the
//    stop code, and return.  Otherwise, the
//    ServiceControlManager will believe that
//    the service has stopped responding.
//    

VOID ServiceStop()
{
	try
	{
		//ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 4 * 60 * 1000);
		ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 3000);
		if ( hServerEvents[0] )
			SetEvent(hServerEvents[0]);

	}
	catch(CException* e)
	{
		e->Delete();
	}

}

