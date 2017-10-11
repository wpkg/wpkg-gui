#include "stdafx.h"
#include "service.h"
#include "..\components\security.h"
#include "..\components\secret.h"
#include "..\components\netconnection.h"
#include "..\components\runprocess.h"
#include "..\components\exceptionex.h"






// internal variables
SERVICE_STATUS          ssStatus;       // current status of the service
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwErr = 0;
TCHAR                   szErr[256];

BOOL bDebug = FALSE;

// main classes for logon, connect and execution cscript.exe
static CSecurity security;
static CSecret secret;
static CNetConnection connection;
static CRunProcess process;


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
    // register our service control handler:
    //
    sshStatusHandle = RegisterServiceCtrlHandler( TEXT(SZSERVICENAME), service_ctrl);

    if (!sshStatusHandle)
        goto cleanup;

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
//  FUNCTION: AddErrorToMessageLog(DWORD dwErr)
//
//  PURPOSE: Allows any thread to log an error message
//
//  PARAMETERS:
//    dwErr - error code
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
VOID AddErrorToMessageLog(DWORD dwErr)
{
    HANDLE  hEventSource;

    if ( !bDebug )
    {
        dwErr = GetLastError();

        // Use event logging to log the error.
        //
        hEventSource = RegisterEventSource(NULL, TEXT(SZSERVICENAME));

        if (hEventSource != NULL) {
            ReportEvent(hEventSource, // handle of event source
                EVENTLOG_ERROR_TYPE,  // event type
                0,                    // event category
                dwErr,                // event ID
                NULL,                 // current user's SID
                0,                    // strings in lpszStrings
                0,                    // no bytes of raw data
                NULL,                 // array of error strings
                NULL);                // no raw data

            (VOID) DeregisterEventSource(hEventSource);
        }
    }
}




//
//  FUNCTION: AddToMessageLog(LPTSTR lpszMsg)
//
//  PURPOSE: Allows any thread to log an error message
//
//  PARAMETERS:
//    lpszMsg - text for message
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
VOID AddToMessageLog(LPTSTR lpszMsg)
{
    TCHAR   szMsg[256];
    HANDLE  hEventSource;
    LPTSTR  lpszStrings[2];


    if ( !bDebug )
    {
        dwErr = GetLastError();

        // Use event logging to log the error.
        //
        hEventSource = RegisterEventSource(NULL, TEXT(SZSERVICENAME));

        _stprintf(szMsg, TEXT("%s error: %d"), TEXT(SZSERVICENAME), dwErr);
        lpszStrings[0] = szMsg;
        lpszStrings[1] = lpszMsg;

        if (hEventSource != NULL) {
            ReportEvent(hEventSource, // handle of event source
                EVENTLOG_ERROR_TYPE,  // event type
                0,                    // event category
                0,                    // event ID
                NULL,                 // current user's SID
                2,                    // strings in lpszStrings
                0,                    // no bytes of raw data
                (LPCTSTR*)lpszStrings,// array of error strings
                NULL);                // no raw data

            (VOID) DeregisterEventSource(hEventSource);
        }
    }
}


VOID AddMessageToMessageLog(LPCTSTR lpszMsg)
{
    HANDLE  hEventSource;
    if ( !bDebug )
    {
        // Use event logging to log the error.
        //
        hEventSource = RegisterEventSource(NULL, TEXT(SZSERVICENAME));

        if (hEventSource != NULL) {
            ReportEvent(hEventSource, // handle of event source
				EVENTLOG_SUCCESS,     // event type
                0,                    // event category
                1,                    // event ID
                NULL,                 // current user's SID
                1,                    // strings in lpszStrings
                0,                    // no bytes of raw data
                &lpszMsg,// array of error strings
                NULL);                // no raw data

            (VOID) DeregisterEventSource(hEventSource);
        }
    }
}

VOID AddErrMsgToMessageLog(LPCTSTR lpszMsg)
{
    HANDLE  hEventSource;
    if ( !bDebug )
    {
        // Use event logging to log the error.
        //
        hEventSource = RegisterEventSource(NULL, TEXT(SZSERVICENAME));

        if (hEventSource != NULL) {
            ReportEvent(hEventSource, // handle of event source
                EVENTLOG_ERROR_TYPE,  // event type
                0,                    // event category
                1,                    // event ID
                NULL,                 // current user's SID
                1,                    // strings in lpszStrings
                0,                    // no bytes of raw data
                &lpszMsg,// array of error strings
                NULL);                // no raw data

            (VOID) DeregisterEventSource(hEventSource);
        }
    }
}





//
//  FUNCTION: service_ctrl
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
VOID WINAPI service_ctrl(DWORD dwCtrlCode)
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
        case SERVICE_CONTROL_STOP:
            ServiceStop();
            return;

        // Update the service status.
        //
        case SERVICE_CONTROL_INTERROGATE:
            break;

        // invalid control code
        //
        default:
            break;

    }

    ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
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
            ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_SHUTDOWN;

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
            password);                      // no password

        if ( schService )
        {
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
			//ServiceStop();
			return TRUE;
        case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
			//ServiceStop();
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

    ///////////////////////////////////////////////////
    //
    // Service initialization
    //


	HANDLE hToken = NULL;

    // report the status to the service control manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_START_PENDING, // service state
        NO_ERROR,              // exit code
        3000))                 // wait hint
	{
		printf("ReportStatusToSCMgr failed\n");
		return;
	}

	security.AllowAdminAccesSa();

	// create the event object. The control handler function signals
    // this event when it receives the "stop" control code.
    //
    hServerEvents[0] = CreateEvent(
        NULL,    // no security attributes
        FALSE,    // manual reset event
        FALSE,   // not-signalled
        NULL);   // no name

    if ( hServerEvents[0] == NULL)
        return;
	

	// manual start event
	hServerEvents[1] = CreateEvent(
		&security.m_sa,     // access for Admins & system
        FALSE,				// manual reset event
        FALSE,				// not-signalled
        "START_WPKG_SRVR");   

    if ( hServerEvents[1] == NULL)
        return;

	// viewer stop event
	hServerEvents[2] = CreateEvent(
		&security.m_sa,     // access for Admins &  system
        FALSE,				// manual reset event
        FALSE,				// not-signalled
        "STOP_WPKG_SRVR");   

    if ( hServerEvents[2] == NULL)
        return;


	// Initialise the Application here
	// or upgrade to the professional version to get the full application thread

    // report the status to the service control manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_RUNNING,       // service state
        NO_ERROR,              // exit code
        0))                    // wait hint
	{
		return;
	}

    //
    // End of initialization
    //
    ////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    //
    // Service is now running, perform work until shutdown
    //

	try
	{

		
		// Load secret data
		secret.LoadSecret();
		// Switch to required security context
		security.LogonUser(secret.m_strScriptExecUser.GetBuffer(),
			secret.m_strScriptExecPassword.GetBuffer(),hToken);

		
		// add message to event log
		AddMessageToMessageLog("Set script security context: successfuly done."); 

		// connect to the remote resource if required
		
		if(PathIsNetworkPath(secret.m_strScriptFile))
		{
			connection.AddConnection(secret.m_strScriptFile,
				secret.m_strScriptConnUser,secret.m_strScriptConnPassword);

			AddMessageToMessageLog("Network resource: successfuly connected"); 
		}

		
		
		CString command("cscript.exe ");
		
		command += secret.m_strScriptFile;
		command += " ";
		command += secret.m_strScriptParameters;

		BOOL OK;

		for(int i=0; i<secret.m_strVarArray.GetCount(); i+=2)
		{
			OK = SetEnvironmentVariable(secret.m_strVarArray.GetAt(i), secret.m_strVarArray.GetAt(i+1));
		}

		if(!secret.m_strPreAction.IsEmpty())
		{
			security.AddDesktopPermission(hToken);
			process.CreateProcess(hToken, secret.m_strPreAction.GetBuffer());
			AddMessageToMessageLog("Script pre action execution: successfuly done");
		}

		// run cscript and wait for termination
		process.CreateProcess(hToken, command.GetBuffer());
		

		AddMessageToMessageLog("Script execution: successfuly done"); 

		if(!secret.m_strPostAction.IsEmpty())
		{
			security.AddDesktopPermission(hToken);
			process.CreateProcess(hToken, secret.m_strPostAction.GetBuffer());
			AddMessageToMessageLog("Script post execution: successfuly done");
		}

		// disconnect from network resource
		if(PathIsNetworkPath(secret.m_strScriptFile))
		{
			connection.Disconnect(secret.m_strScriptFile);
			AddMessageToMessageLog("Network resource: successfuly disconnected"); 
		}
		
	}
	catch(CException* exc)
	{
		char message[1024];
		exc->GetErrorMessage(message,1024);
		AddErrMsgToMessageLog(message);
		exc->Delete();
		ServiceStop();
		goto cleanup;
	}


	
	while(TRUE)
	{
        DWORD dwWait = WaitForMultipleObjects(3, hServerEvents, FALSE,  INFINITE );
        if ( dwWait == WAIT_OBJECT_0 )       // not overlapped i/o event - error occurred,
            break;                           // or server stop signaled

		if ( dwWait == WAIT_OBJECT_0+1 )     // manual start event
		{
			try
			{
				//... feature for manual start signal
				
			}
			catch(CException* e)
			{
				e->Delete();
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

cleanup:

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
		ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 3000);

		//...
		
		if ( hServerEvents[0] )
			SetEvent(hServerEvents[0]);

	}
	catch(CException* e)
	{
		e->Delete();
	}

}

