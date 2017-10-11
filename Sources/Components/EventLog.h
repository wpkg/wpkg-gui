#pragma once
#include "..\components\Report.h"

class CEventLog
{
public:
	
	static CString m_strAppName;
	static CString m_PathToLogFile;
	CEventLog(void);
	virtual ~CEventLog(void);
	static void AddMessageToLog(LPCTSTR lpszMsg);
	static void AddErrorMessageToLog(LPCTSTR lpszMsg);
private:
	static void AddEntryToLog(WORD wType, LPCTSTR lpszMsg);
};
