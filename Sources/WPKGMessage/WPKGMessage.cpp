// WPKGMessage.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WPKGMessage.h"
#include "..\components\filemap.h"
#include "WPKGMessageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL bWorking = TRUE;
BOOL bConnected = FALSE;

CEvent eTestServiceStatus;




UINT WaitForDone(LPVOID)
{
	DWORD dwTimeOut = 60; // in seconds
	CFileMap startTimeInfo;


	HKEY phkResult;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Notify\\WPKGLogon",
		0,
		KEY_READ,
		&phkResult );

	dwTimeOut = 60;
	DWORD cbData = 4;
	DWORD Type;
	
	// in registry timeout is in seconds
	DWORD OK = RegQueryValueEx( phkResult,
		"MaxWait", 0, &Type, (BYTE*)&dwTimeOut, &cbData );

	RegCloseKey(phkResult); 


	// logon delay must be longer then wpkgmessage wait
	if(dwTimeOut >= 90)
		dwTimeOut -= 30;
	
	
	CTime date;
	CTime current = date = CTime::GetCurrentTime();
	CTimeSpan ts;

	DWORD timeout = 0;


	while(timeout<dwTimeOut)
	{
		try
		{
			startTimeInfo.OpenSharedMem();
			date = startTimeInfo.ReadStartDate();
			bConnected = TRUE;
			break;
		}
		catch(CException* e)
		{
			e->Delete();
			Sleep(5000);
			timeout+=5;
			eTestServiceStatus.SetEvent();
			continue;
		}
	}

	if(dwTimeOut>timeout)
		dwTimeOut -= timeout;

	
	ts = current - date;
	LONGLONG timeElapsed = ts.GetTotalSeconds();


	if(timeElapsed < dwTimeOut )
	{
		eTestServiceStatus.SetEvent();

		if(timeElapsed<dwTimeOut && timeElapsed>0)
			dwTimeOut-=(DWORD)timeElapsed;


		dwTimeOut *= 1000;

		HANDLE hWorking = OpenEvent(
	        SYNCHRONIZE,
		    FALSE,
			"Global\\WORKING_WPKG_SRVR");   // name


		WaitForSingleObject(hWorking,dwTimeOut);
		if(hWorking!=INVALID_HANDLE_VALUE)
			CloseHandle(hWorking);
	}

	bWorking = FALSE;

	eTestServiceStatus.SetEvent();


	return 0;
}




BOOL IsWorking()
{
	return bWorking;
}

BOOL IsWorkingDone()
{
	return (!bWorking && bConnected);
}


// CWPKGMessageApp

BEGIN_MESSAGE_MAP(CWPKGMessageApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWPKGMessageApp construction

CWPKGMessageApp::CWPKGMessageApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CWPKGMessageApp object

CWPKGMessageApp theApp;


// CWPKGMessageApp initialization

BOOL CWPKGMessageApp::InitInstance()
{
	CWinApp::InitInstance();

	CWinThread* pThr = AfxBeginThread(WaitForDone,NULL);
	
	CSingleLock lck(&eTestServiceStatus);
	lck.Lock();

	if(!IsWorkingDone())
	{
		CWPKGMessageDlg dlg;
		m_pMainWnd = &dlg;
		dlg.DoModal();
	}

	lck.Unlock();
	eTestServiceStatus.Unlock();

	WaitForSingleObject(pThr->m_hThread,INFINITE);

	return FALSE;
}
