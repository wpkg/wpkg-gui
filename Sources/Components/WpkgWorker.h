#pragma once

#include "security.h"
#include "secret.h"
#include "netconnection.h"
#include "runprocess.h"
#include "filemap.h"
#include "exceptionex.h"
#include "serverping.h"
#include "ProgressMessage.h"
#include "EventLog.h"


class CWpkgWorker
{
private:
	BOOL m_bRestartSystem;
	// main classes for logon, connect and execution cscript.exe
	CSecurity security;
	CSecret secret;
	CNetConnection connection;
	CRunProcess process;
	CFileMap m_wokerProgressInfo;
	CProgressMessage progressMessage;
	void DoAction();
	int m_iRepeatAction;

public:
	void TerminateProcesses();
	void WakeUpProcesses();
	void PerformRestart(int iLoggedUsers);
	BOOL IsMustStop();
	BOOL IsRunOnShutdown();
	CWpkgWorker(void);
	virtual ~CWpkgWorker(void);
	void WpkgClientAction();
	void Cleanup();
	void Initialize(void);
};
