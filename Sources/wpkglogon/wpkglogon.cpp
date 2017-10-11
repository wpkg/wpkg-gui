// wpkglogon.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Winwlx.h"
#include "wpkglogon.h"
#include <stdio.h>

typedef DWORD  (__stdcall *CSI)  (void);


static CSI WTSGetActiveConsoleSessionIdEx;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	
	
    return TRUE;
}


static DWORD CreateMessage(char* commandLine)
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
		NULL,					// Use parent’s environment block. 
		NULL,
		&si,					// Pointer to STARTUPINFO structure.
		&pi );					// Pointer to PROCESS_INFORMATION structure.



    // Close process and thread handles. 
	WaitForSingleObject(pi.hProcess,INFINITE);

	DWORD exitCode = 0;
	GetExitCodeProcess(pi.hProcess,&exitCode);

    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
	Sleep(100);

	return exitCode;
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

	// not all operating systems support terminal services ->
	HMODULE hModule = LoadLibrary("Kernel32.dll");
	WTSGetActiveConsoleSessionIdEx = NULL;

	if(hModule)
		WTSGetActiveConsoleSessionIdEx = (CSI)GetProcAddress(hModule,"WTSGetActiveConsoleSessionId");
	DWORD sessionId = 0;
	
	if(WTSGetActiveConsoleSessionIdEx)
		sessionId = WTSGetActiveConsoleSessionIdEx();

	if(hModule)
		FreeLibrary(hModule);

	// <- term. serv.
	
	int cleantBoot = GetSystemMetrics(SM_CLEANBOOT);
	int remoteSession = GetSystemMetrics(SM_REMOTESESSION);

	if(dwTimeOut>0 && cleantBoot==0 && sessionId==0 && remoteSession==0)
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


