#include "stdafx.h"
#include "service.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

#ifndef _DEBUG
#ifdef _DEBUG_SERVICE
#error _DEBUG_SERVICE has been defined in a release build.  This could be dangerous - check settings.
#endif
#endif

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { TEXT(SZSERVICENAME), (LPSERVICE_MAIN_FUNCTION)service_main },
        { NULL, NULL }
    };


	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		printf("Fatal Error: MFC initialization failed\n");
		nRetCode = 1;
		return nRetCode;
	}

	if ( (argc > 1) &&
         ((*argv[1] == '-') || (*argv[1] == '/')) )
    {
        if ( _stricmp( "install", argv[1]+1 ) == 0 )
        {
            CmdInstallService();
        }
        else if ( _stricmp( "remove", argv[1]+1 ) == 0 )
        {
            CmdRemoveService();
        }
        else if ( _stricmp( "debug", argv[1]+1 ) == 0 )
        {
            bDebug = TRUE;
            CmdDebugService(argc, argv);
        }
        else
        {
            printf( "%s -install          to install the service\n", SZAPPNAME );
			printf( "%s -remove           to remove the service\n", SZAPPNAME );
			printf( "%s -debug			  to run as a console app for debugging\n", SZAPPNAME );
			Sleep(10000);
			exit(0);
        }
		exit(0);
    
    }

    // if it doesn't match any of the above parameters
    // the service control manager may be starting the service
    // so we must call StartServiceCtrlDispatcher
    // this is just to be friendly

    printf( "\nStartServiceCtrlDispatcher being called.\n" );
    printf( "This may take several seconds.  Please wait.\n" );

    if (!StartServiceCtrlDispatcher(dispatchTable))
		AddToMessageLog(TEXT("StartServiceCtrlDispatcher failed."));
	
	return nRetCode;
}

