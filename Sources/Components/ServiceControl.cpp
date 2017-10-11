#include "StdAfx.h"
#include "ServiceControl.h"
#include "Winsvc.h"
#include "AppTool.h"


#define SZSERVICENAME        "WpkgService"
#define SZSERVICEDISPLAYNAME "WPKG Service"

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



CServiceControl::CServiceControl(void)
{
}

CServiceControl::~CServiceControl(void)
{
}

void CServiceControl::SetPreShutdownTimeout(DWORD dwTimeout)
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
			SERVICE_PRESHUTDOWN_INFO shInfo;
			shInfo.dwPreshutdownTimeout = dwTimeout;
			ChangeServiceConfig2(schService,SERVICE_CONFIG_PRESHUTDOWN_INFO,&shInfo); 

			CloseServiceHandle(schService);

		}


		CloseServiceHandle(schSCManager);

	}



}

CString CServiceControl::ServiceControl(CString strServerName, BOOL bStop, CEdit* pEditProgressMessage)
{
	DWORD                   dwErr = 0;
	TCHAR                   szErr[256];
	SERVICE_STATUS          ssStatus;
	SC_HANDLE				schService;
	SC_HANDLE				schSCManager;
	CString str;


	CWaitCursor wait;
	DWORD dwControlCode = SERVICE_CONTROL_STOP;
	DWORD dwCurrentState = SERVICE_STOP_PENDING;

	CString str1,str2,str3;
	if(bStop)
	{
		str1 = "Stopping";
		str2 = "stoped";
		str3 = "stop";
	}
	else
	{
		str1 = "Starting";
		str2 = "started";
		str3 = "start";
	}
	BOOL bResult = FALSE;


	schSCManager = OpenSCManager(
		strServerName,        // machine (NULL == local)
		NULL,                   // database (NULL == default)
		SC_MANAGER_ALL_ACCESS   // access required
		);

	if ( schSCManager )
	{
		schService = OpenService(schSCManager, TEXT(SZSERVICENAME), SERVICE_ALL_ACCESS);

		if (schService)
		{
			// try to stop the service

			if(bStop)
				bResult = ControlService( schService, dwControlCode, &ssStatus );
			else
				bResult = StartService( schService, 0, NULL);

			if ( bResult )
			{
				str.Format("%s: %s %s.", strServerName, str1, TEXT(SZSERVICEDISPLAYNAME));
				pEditProgressMessage->SetWindowTextA(str);
				Sleep( 1000 );

				while( QueryServiceStatus( schService, &ssStatus ) )
				{
					if ( ssStatus.dwCurrentState == dwCurrentState )
					{

						str+=".";
						pEditProgressMessage->SetWindowTextA(str);
						pEditProgressMessage->UpdateWindow();
						if(str.GetLength()>113)
							str.Remove('.');
						CAppTool::PumpMessages();
						Sleep( 500 );
					}
					else
						break;
				}

				if ( ssStatus.dwCurrentState == (bStop?SERVICE_STOPPED:SERVICE_RUNNING) )
				{
					str.Format("%s: %s %s.", strServerName, TEXT(SZSERVICEDISPLAYNAME),str2);
					pEditProgressMessage->SetWindowTextA(str);
				}
				else
				{
					str.Format("%s: %s failed to %s.", strServerName, TEXT(SZSERVICEDISPLAYNAME),str3);
					pEditProgressMessage->SetWindowTextA(str);
				}

			}
			else
			{
				str.Format("%s: %s failed to %s - %s.", strServerName, TEXT(SZSERVICEDISPLAYNAME),str3, GetLastErrorText(szErr,256));
				pEditProgressMessage->SetWindowTextA(str);
				CloseServiceHandle(schService);
			}
		}
		else
		{
			str.Format("%s: open %s failed - %s.", strServerName, TEXT(SZSERVICEDISPLAYNAME),GetLastErrorText(szErr,256));
			pEditProgressMessage->SetWindowTextA(str);
		}

		CloseServiceHandle(schSCManager);
	}
	else
	{
		str.Format("%s: open Service Control Manager failed - %s", strServerName, GetLastErrorText(szErr,256));
		pEditProgressMessage->SetWindowTextA(str);
		
		
	}

	return str;
}
