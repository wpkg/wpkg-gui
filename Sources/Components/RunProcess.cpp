#include "StdAfx.h"
#include ".\runprocess.h"
#include "exceptionex.h"



CRunProcess::CRunProcess(void)
{
}

CRunProcess::~CRunProcess(void)
{
}


void CRunProcess::CreateProcess(HANDLE hToken, char* commandLine, WORD dwShow)
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
	si.lpDesktop = "WinSta0\\Default"; 
	si.dwX = si.dwY = si.dwXSize = si.dwYSize = 0L; 
	si.dwFlags = STARTF_USESHOWWINDOW; 
	si.wShowWindow = dwShow; 
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
			NORMAL_PRIORITY_CLASS,	// No creation flags. 
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
			NORMAL_PRIORITY_CLASS,	// No creation flags. 
			NULL,					// Use parent’s environment block. 
			NULL,
			&si,					// Pointer to STARTUPINFO structure.
			&pi );					// Pointer to PROCESS_INFORMATION structure.
		   
	}

	if(!OK)
	{
		Error = GetLastError();
		CExceptionEx::ThrowError(Error);
	}
    

    // Close process and thread handles. 
	WaitForSingleObject(pi.hProcess,INFINITE);

    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
	Sleep(100);

}

