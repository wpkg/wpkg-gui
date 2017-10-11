#include "StdAfx.h"
#include ".\runprocess.h"
#include "exceptionex.h"


//DWORD CRunProcess::m_dwStartMode;
//DWORD CRunProcess::m_dwRestartCount;
//CEventSender CRunProcess::m_eventSender;



CRunProcess::CRunProcess(void)
{
}

CRunProcess::~CRunProcess(void)
{
}

BOOL CRunProcess::PreventSystemShutdown()
{
   HANDLE hToken;              // handle to process token 
   TOKEN_PRIVILEGES tkp;       // pointer to token structure 
 
   // Get the current process token handle  so we can get shutdown 
   // privilege. 
 
   if (!OpenProcessToken(GetCurrentProcess(), 
         TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
      return FALSE; 
 
   // Get the LUID for shutdown privilege. 
 
   LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
         &tkp.Privileges[0].Luid); 
 
   tkp.PrivilegeCount = 1;  // one privilege to set    
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
   // Get shutdown privilege for this process. 
 
   AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
        (PTOKEN_PRIVILEGES)NULL, 0); 
 
   if (GetLastError() != ERROR_SUCCESS) 
      return FALSE; 
 
   // Prevent the system from shutting down. 
 
   if ( !AbortSystemShutdown(NULL) ) 
      return FALSE; 
 
   // Disable shutdown privilege. 
 
   tkp.Privileges[0].Attributes = 0; 
   AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
       (PTOKEN_PRIVILEGES) NULL, 0); 
 
   return TRUE;
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
		"Count", 0, &Type, (BYTE*)&m_dwRestartCount, &cbData );

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
		"Count", 0, REG_DWORD, (BYTE*)&m_dwRestartCount, 4 );


	RegCloseKey(phkResult); 


}


void CRunProcess::RestartSystem(char* message, int countOfLoggedUsers)
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
	if(countOfLoggedUsers>0)
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
	{
		si.wShowWindow = SW_SHOW;
		si.lpDesktop = "WinSta0\\Default"; 
	}
	else
	{
		si.wShowWindow = SW_HIDE; 
		si.lpDesktop = ""; 
	}

	si.dwX = si.dwY = si.dwXSize = si.dwYSize = 0L; 
	si.dwFlags = STARTF_USESHOWWINDOW; 

	
	//si.wShowWindow = SW_SHOW; 
	si.lpReserved2 = NULL; 
	si.cbReserved2 = 0; 


	CEventArgs e;
	e.m_EventType = e.typeHandle;
	m_eventSender.FireEvent(NULL,e);

	si.hStdOutput = e.m_hFile;
	si.hStdError = e.m_hFile;
	if(si.hStdOutput)
		si.dwFlags |= STARTF_USESTDHANDLES; 


	BOOL OK=FALSE;

	if(hToken)
	{
		OK = ::CreateProcessAsUser( hToken,
			NULL,                   // No module name (use command line). 
			command,			    // Command line. 
			NULL,					// Process handle not inheritable. 
			NULL,					// Thread handle not inheritable. 
			TRUE,					// Set handle inheritance to FALSE. 
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
			TRUE,					// Set handle inheritance to FALSE. 
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


	
	e.m_EventType = e.typeMessage;
	m_eventSender.FireEvent(NULL,e);
    

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

/*
HANDLE hTokenNew = NULL, hTokenDup = NULL;
 HMODULE  hmod = LoadLibrary("kernel32.dll");
 WTSGETACTIVECONSOLESESSIONID lpfnWTSGetActiveConsoleSessionId = (WTSGETACTIVECONSOLESESSIONID)GetProcAddress(hmod,"WTSGetActiveConsoleSessionId"); 
 DWORD dwSessionId = lpfnWTSGetActiveConsoleSessionId();
 WTSQueryUserToken(dwSessionId, &hToken);
 DuplicateTokenEx(hTokenNew,MAXIMUM_ALLOWED,NULL,SecurityIdentification,TokenPrimary,&hTokenDup);
 //
 WriteToLog("Calling lpfnCreateEnvironmentBlock");
 ZeroMemory( &si, sizeof( STARTUPINFO ) );
 si.cb = sizeof( STARTUPINFO );
 si.lpDesktop = "winsta0\\default";


 LPVOID  pEnv = NULL;
 DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;
 HMODULE hModule = LoadLibrary("Userenv.dll");
 if(hModule )
 {
  LPFN_CreateEnvironmentBlock lpfnCreateEnvironmentBlock = (LPFN_CreateEnvironmentBlock)GetProcAddress( hModule, "CreateEnvironmentBlock" );
  if( lpfnCreateEnvironmentBlock != NULL )
  {
   if(lpfnCreateEnvironmentBlock(&pEnv, hTokenDup, FALSE))
   {
    WriteToLog("CreateEnvironmentBlock Ok");
    dwCreationFlag |= CREATE_UNICODE_ENVIRONMENT;    
   }
   else
   {
    pEnv = NULL;
   }
  }
 }
  //
 ZeroMemory( &pi,sizeof(pi));
 
 if ( !CreateProcessAsUser(
  hTokenDup,
  NULL,
  ( char * )pszCmd,  
  NULL,
  NULL,
  FALSE,
  dwCreationFlag,
  pEnv,
  NULL,
  &si,
  &pi
  ) )
 {
  
  goto RESTORE;
 } 



*/