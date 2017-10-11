// wpkglogon.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "wpkglogon.h"
#include "Winwlx.h"
#include "..\components\secret.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef DWORD  (__stdcall *CSI)  (void);

static CSI WTSGetActiveConsoleSessionIdEx;



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
	DWORD exitCode = 0;
	
	WaitForSingleObject(pi.hProcess,INFINITE);


	GetExitCodeProcess(pi.hProcess,&exitCode);

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	Sleep(100);

	return exitCode;
}


extern "C" void WINAPI WLEventStartup (PWLX_NOTIFICATION_INFO pInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CSecret secret;
	secret.m_bRunOnShutdown = FALSE;
	try
	{
		::CoInitialize(NULL);
		secret.LoadSecret();
	}
	catch(CException* e)
	{
		e->Delete();
	}

	::CoUninitialize();
	if(secret.m_bRunOnShutdown)
		return;


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

extern "C" void WINAPI WLEventShutdown (PWLX_NOTIFICATION_INFO pInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CSecret secret;
	
	secret.m_bRunOnShutdown = FALSE;
	try
	{
		::CoInitialize(NULL);
		secret.LoadSecret();
	}
	catch(CException* e)
	{
		e->Delete();
	}

	::CoUninitialize();
	if(!secret.m_bRunOnShutdown)
		return;


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
	
		HANDLE  hStartEvent = NULL;

		hStartEvent = CreateEvent(
			NULL,     
			FALSE,				// manual reset event
			FALSE,				// not-signalled
			"Global\\START_WPKG_SRVR");   

		if ( hStartEvent )
		{
			SetEvent(hStartEvent);
		}


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

		//CreateMessage("D:\\Microsoft Visual Studio 8\\Common7\\Tools\\Bin\\winnt\\Dbmon.Exe");
		strcat(value,"WPKGMessage.exe");
		CreateMessage(value);
		
		if ( hStartEvent )
		{
			CloseHandle(hStartEvent);
		}

	}


}



// CwpkglogonApp

BEGIN_MESSAGE_MAP(CwpkglogonApp, CWinApp)
END_MESSAGE_MAP()


// CwpkglogonApp construction

CwpkglogonApp::CwpkglogonApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CwpkglogonApp object

CwpkglogonApp theApp;


// CwpkglogonApp initialization

BOOL CwpkglogonApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
