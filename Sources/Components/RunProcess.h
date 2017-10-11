#pragma once
#include "EventHandler.h"

class CRunProcess
{
public:
	CRunProcess(void);
	DWORD m_dwStartMode;
	DWORD m_dwRestartCount;
	CEventSender m_eventSender;

	virtual ~CRunProcess(void);
	void ReadRestartInfo();
	void WriteRestartInfo(void);
	BOOL PreventSystemShutdown();
	void RestartSystem(char* message, int coutOfLoggedUsers);
	DWORD CreateProcess(HANDLE hToken, char* commandLine,
		BOOL bShowGUI = TRUE, DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS, DWORD dwWait = INFINITE);

};
