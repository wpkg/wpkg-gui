#pragma once
#include "EventHandler.h"

class CRunProcess
{
private:
	HANDLE m_hTerminateProcess;
	BOOL m_bTerminate;
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
	void TerminateProcess();
	void WakeUpProcess();
	BOOL IsNowTerminated();
	DWORD CreateProcess(HANDLE hToken, char* commandLine,
		BOOL bShowGUI = TRUE, DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS, DWORD dwWait = INFINITE);

};
