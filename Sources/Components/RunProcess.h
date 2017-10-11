#pragma once

class CRunProcess
{
public:
	CRunProcess(void);
	static DWORD m_dwStartMode;
	static DWORD m_dwRestartCount;

	virtual ~CRunProcess(void);
	static void ReadRestartInfo();
	static void WriteRestartInfo(void);
	static void RestartSystem(char* message, int coutOfLoggedUsers);
	static DWORD CreateProcess(HANDLE hToken, char* commandLine,
		BOOL bShowGUI = TRUE, DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS, DWORD dwWait = INFINITE );

};
