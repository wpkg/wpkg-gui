// wpkglogon.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Winwlx.h"
#include "wpkglogon.h"
#include <stdio.h>


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}


static void CreateMessage(char* commandLine)
{
	char command[4096];
	ZeroMemory(command,4096);
	strcat(command,"\"");
	strcat(command,commandLine);
	strcat(command,"\"");

	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	
    ZeroMemory( &si, sizeof(si) );
	ZeroMemory( &pi, sizeof(pi) );

	si.cb = sizeof(STARTUPINFO); 
	si.lpReserved = NULL; 
	si.lpTitle = NULL; 

	si.lpDesktop = "WinSta0\\Winlogon"; 
	
	si.dwX = si.dwY = si.dwXSize = si.dwYSize = 0L; 
	si.dwFlags = STARTF_USESHOWWINDOW; 
	si.wShowWindow = SW_SHOW; 
	si.lpReserved2 = NULL; 
	si.cbReserved2 = 0; 

	BOOL OK=FALSE;

	OK = ::CreateProcess( NULL,	// No module name (use command line). 
		command,				// Command line. 
		NULL,					// Process handle not inheritable. 
		NULL,					// Thread handle not inheritable. 
		FALSE,					// Set handle inheritance to FALSE. 
		NORMAL_PRIORITY_CLASS,	// No creation flags. 
		NULL,					// Use parent�s environment block. 
		NULL,
		&si,					// Pointer to STARTUPINFO structure.
		&pi );					// Pointer to PROCESS_INFORMATION structure.



    // Close process and thread handles. 
	WaitForSingleObject(pi.hProcess,INFINITE);

    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
	Sleep(100);
}


extern "C" void WINAPI WLEventStartup (PWLX_NOTIFICATION_INFO pInfo)
{
	

	HKEY phkResult;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Notify\\WPKGLogon",
		0,
		KEY_READ,
		&phkResult );

	DWORD dwTimeOut = 0;
	DWORD cbData = 4;
	DWORD Type;
	
	DWORD OK = RegQueryValueEx( phkResult,
		"MaxWait", 0, &Type, (BYTE*)&dwTimeOut, &cbData );

	RegCloseKey(phkResult); 

	if(dwTimeOut>0)
	{

		RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			"SOFTWARE\\WPKG.ORG\\Settings",
			0,
			KEY_READ,
			&phkResult );

		char value[1024];
		ZeroMemory(value,1024);

		DWORD size = 1024;
		
		DWORD cbData = 1024;
		DWORD Type;
		
		DWORD OK = RegQueryValueEx( phkResult,
			"Path", 0, &Type, (BYTE*)value, &cbData );
		RegCloseKey(phkResult); 

		strcat(value,"WPKGMessage.exe");

		CreateMessage(value);
	}
}


