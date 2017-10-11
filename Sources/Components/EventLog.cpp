#include "stdafx.h"
#include "EventLog.h"


CString CEventLog::m_strAppName;
CString CEventLog::m_PathToLogFile;


CEventLog::CEventLog(void)
{
}

CEventLog::~CEventLog(void)
{
}

void CEventLog::AddEntryToLog(WORD wType, LPCTSTR lpszMsg)
{
	TRACE1("%s\n",lpszMsg);
	HANDLE  hEventSource;
	// Use event logging to log the error.
	//
	hEventSource = RegisterEventSource(NULL, m_strAppName);

	if (hEventSource != NULL)
	{
		ReportEvent(hEventSource, // handle of event source
			wType,                // event type
			0,                    // event category
			1,                    // event ID
			NULL,                 // current user's SID
			1,                    // strings in lpszStrings
			0,                    // no bytes of raw data
			&lpszMsg,// array of error strings
			NULL);                // no raw data

		(VOID) DeregisterEventSource(hEventSource);
	}


	if(!m_PathToLogFile.IsEmpty())
	{
		CReport report;
		report.m_bFlush = TRUE;
		report.OpenReport(m_PathToLogFile);
		report.AddInfo(lpszMsg);

		report.Close();
	}



}

void CEventLog::AddMessageToLog(LPCTSTR lpszMsg)
{
	AddEntryToLog(EVENTLOG_INFORMATION_TYPE, lpszMsg);
}

void CEventLog::AddErrorMessageToLog(LPCTSTR lpszMsg)
{
	AddEntryToLog(EVENTLOG_ERROR_TYPE, lpszMsg);
}
