#include "StdAfx.h"
#include ".\runprocess.h"
#include "exceptionex.h"


DWORD CRunProcess::m_dwStartMode;
DWORD CRunProcess::m_dwRestartCount;



CRunProcess::CRunProcess(void)
{
}

CRunProcess::~CRunProcess(void)
{
}

void CRunProcess::ReadRestartInfo(void)
{
	CRunProcess::m_dwRestartCount = 0;

	// 0 start normally, 1 - reboot
	CRunProcess::m_dwStartMode = 0;

	HKEY phkResult;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\WPKG.ORG\\Restart",
		0,
		KEY_READ,
		&phkResult );

	DWORD cbData = 4;
	DWORD Type;
	
	//RegQueryValueEx( phkResult,
	//	"Mode", 0, &Type, (BYTE*)&CRunProcess::m_dwStartMode, &cbData );

	cbData = 4;
	RegQueryValueEx( phkResult,
		"Count", 0, &Type, (BYTE*)&CRunProcess::m_dwRestartCount, &cbData );

	RegCloseKey(phkResult); 

}

void CRunProcess::WriteRestartInfo(void)
{
	HKEY phkResult;
	DWORD lpdwDisposition = REG_CREATED_NEW_KEY;



	RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\WPKG.ORG\\Restart",
		0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&phkResult,
		&lpdwDisposition);


	//RegSetValueEx( phkResult,
	//	"Mode", 0, REG_DWORD, (BYTE*)&CRunProcess::m_dwStartMode, 4 );

	RegSetValueEx( phkResult,
		"Count", 0, REG_DWORD, (BYTE*)&CRunProcess::m_dwRestartCount, 4 );


	RegCloseKey(phkResult); 


}


void CRunProcess::RestartSystem(char* message, int coutOfLoggedUsers)
{
	RevertToSelf();

	HANDLE hToken=NULL; 
	TOKEN_PRIVILEGES tkp; 
	DWORD error = 0;
 
	// Get a token for this process. 
 
	BOOL OK = OpenProcessToken(GetCurrentProcess(), 
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	if(!OK)
	{
		error = GetLastError();

		if(error)
			CExceptionEx::ThrowError("RestartSystem->OpenProcessToken",error);
	}

    
 
	// Get the LUID for the shutdown privilege. 
 
	OK = LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
        &tkp.Privileges[0].Luid); 

	if(!OK)
	{
		error = GetLastError();

		if(error)
			CExceptionEx::ThrowError("RestartSystem->LookupPrivilegeValue",error);
	}

 
	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
	// Get the shutdown privilege for this process. 
 
	OK = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
        (PTOKEN_PRIVILEGES)NULL, 0); 

	if(!OK)
	{
		error = GetLastError();

		if(error)
			CExceptionEx::ThrowError("RestartSystem->AdjustTokenPrivileges",error);
	}


	
 
	// Shut down the system and force all applications to close. 

	SetLastError(0);

	DWORD dwTimeOut = 3;
	if(coutOfLoggedUsers>0)
		dwTimeOut = 120;

	OSVERSIONINFO osi;
	osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	OK = GetVersionEx( &osi );

	if(osi.dwMajorVersion<5)
		dwTimeOut = 120;
	if(osi.dwMajorVersion=5 && osi.dwMinorVersion<1)
		dwTimeOut = 120;


	OK = InitiateSystemShutdown( NULL, message, 
		dwTimeOut, 1, 1 ); 	

	if(!OK)
	{
		error = GetLastError();

		if(error)
			CExceptionEx::ThrowError("RestartSystem->InitiateSystemShutdown",error);
	}

	tkp.Privileges[0].Attributes = 0; 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
        (PTOKEN_PRIVILEGES) NULL, 0); 
 
	
}



DWORD CRunProcess::CreateProcess(HANDLE hToken, char* commandLine, BOOL bShowGUI,
								 DWORD dwCreationFlags, DWORD dwWait)
{

	char command[4096];
	DWORD Error;
	ZeroMemory(command,4096);
	strcat(command,commandLine);

	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	
    ZeroMemory( &si, sizeof(si) );
	ZeroMemory( &pi, sizeof(pi) );

	si.cb = sizeof(STARTUPINFO); 
	si.lpReserved = NULL; 
	si.lpTitle = NULL; 

	if(bShowGUI)
		si.lpDesktop = "WinSta0\\Default"; 
	else
		si.lpDesktop = ""; 

	si.dwX = si.dwY = si.dwXSize = si.dwYSize = 0L; 
	si.dwFlags = STARTF_USESHOWWINDOW; 
	si.wShowWindow = SW_SHOW; 
	si.lpReserved2 = NULL; 
	si.cbReserved2 = 0; 

	BOOL OK=FALSE;

	if(hToken)
	{
		OK = ::CreateProcessAsUser( hToken,
			NULL,                   // No module name (use command line). 
			command,			    // Command line. 
			NULL,					// Process handle not inheritable. 
			NULL,					// Thread handle not inheritable. 
			FALSE,					// Set handle inheritance to FALSE. 
			dwCreationFlags,	    // Creation flags. 
			NULL,					// Use parent’s environment block. 
			NULL,
			&si,					// Pointer to STARTUPINFO structure.
			&pi );					// Pointer to PROCESS_INFORMATION structure.
	}
	else
	{
		OK = ::CreateProcess( NULL,	// No module name (use command line). 
			command,				// Command line. 
			NULL,					// Process handle not inheritable. 
			NULL,					// Thread handle not inheritable. 
			FALSE,					// Set handle inheritance to FALSE. 
			dwCreationFlags,	    // Creation flags. 
			NULL,					// Use parent’s environment block. 
			NULL,
			&si,					// Pointer to STARTUPINFO structure.
			&pi );					// Pointer to PROCESS_INFORMATION structure.
		   
	}

	if(!OK)
	{
		Error = GetLastError();
		CExceptionEx::ThrowError("CreateProcess",Error);
	}
    

    // Close process and thread handles. 
	DWORD exitCode = 0;
	DWORD dwResult = WaitForSingleObject(pi.hProcess,dwWait);
	if(WAIT_TIMEOUT==dwResult)
	{
		exitCode = 2;
		TerminateProcess(pi.hProcess,2);
	}
	else	
		GetExitCodeProcess(pi.hProcess,&exitCode);

    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
	Sleep(100);

	

	
	return exitCode;

}

