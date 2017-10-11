// TestSettings.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "TestSettings.h"
#include ".\testsettings.h"
#include "..\components\secret.h"
#include "..\components\security.h"
#include "..\components\netconnection.h"
#include "..\components\exceptionex.h"
#include "..\components\serverping.h"
#include "..\components\runprocess.h"


// CTestSettings dialog

IMPLEMENT_DYNAMIC(CTestSettings, CDialog)
CTestSettings::CTestSettings(CWnd* pParent /*=NULL*/)
: CDialog(CTestSettings::IDD, pParent)
, m_strResultLog(_T(""))
{
	m_iCountOK=m_iCountFailed=m_iCountTotal=0;
}

CTestSettings::~CTestSettings()
{
}

void CTestSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_Result);
	DDX_Text(pDX, IDC_EDIT_SUMMARY, m_strResultLog);
}


BEGIN_MESSAGE_MAP(CTestSettings, CDialog)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SAVE_LOG, OnBnClickedButtonSaveLog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CLIPBOARD, OnBnClickedButtonSaveClipboard)
END_MESSAGE_MAP()


// CTestSettings message handlers

BOOL CTestSettings::OnInitDialog()
{
	CDialog::OnInitDialog();


	m_Result.SetExtendedStyle(LVS_EX_LABELTIP);




	// TODO:  Add extra initialization here
	m_Result.InsertColumn(0,"Action",LVCFMT_LEFT,250);
	m_Result.InsertColumn(1,"Result",LVCFMT_LEFT,80);
	m_Result.InsertColumn(2,"Message",LVCFMT_LEFT,300);


	SetTimer(1,1500,NULL);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void CTestSettings::DoAppEvents(void)
{
	MSG msg;
	while(PeekMessage(&msg, GetSafeHwnd(), NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	Sleep(1000);
}

void CTestSettings::TryPing()
{
	m_iCountTotal++;

	if(!CSecret::m_bLaptopMode || CSecret::m_iServerTestingMethod == 1)
		return;

	CServerPing s;

	CString strAction;
	strAction.Format("Offline mode - connecting to %s",CSecret::m_strServerIP);
	int itemPos = AddItem(strAction);
	DoAppEvents();

	if(s.WaitForConnect(CSecret::m_strServerIP,CSecret::m_dwServerPingTimeout))
	{
		m_iCountOK++;
		m_Result.SetItemText(itemPos,1,"[OK]");
	}
	else
	{
		m_iCountFailed++;
		m_Result.SetItemText(itemPos,1,"[FAILED]");
	}
}

void CTestSettings::TryConnect(CString path)
{
	m_iCountTotal++;

	if(!PathIsNetworkPath(path))
		return;
	CString strAction;
	strAction.Format("Connecting to %s as %s",path,CSecret::m_strScriptConnUser);
	int itemPos = AddItem(strAction);
	DoAppEvents();
	char message[1024];
	ZeroMemory(message,1024);
	CSecurity s;
	CString strResult;

	CNetConnection connection;
	try
	{

		
		connection.AddConnection(path,
			CSecret::m_strScriptConnUser,CSecret::m_strScriptConnPassword);

		strResult = "[OK]";
		m_iCountOK++;
		RevertToSelf();
	}
	catch(CException* e)
	{
		e->GetErrorMessage(message,1024);
		e->Delete();
		m_iCountFailed++;
		strResult = "[FAILED]";

	}
	m_Result.SetItemText(itemPos,1,strResult);
	m_Result.SetItemText(itemPos,2,message);
	DoAppEvents();

}


void CTestSettings::TryRead(CString path)
{
	m_iCountTotal++;

	if(path.IsEmpty())
		return;

	CString strAction;
	strAction.Format("Trying to read %s",path);
	int itemPos = AddItem(strAction);
	DoAppEvents();
	

	char message[1024];
	ZeroMemory(message,1024);
	CSecurity s;
	CString strResult;


	try
	{
		CFile f;
		CFileException fe;
		if(f.Open(path,CFile::modeRead,&fe))
		{
			char buf[1024];
			f.Read(buf,1024);
			f.Close();
		}
		else
			CFileException::ThrowOsError(fe.m_lOsError,fe.m_strFileName);

		m_iCountOK++;
		strResult = "[OK]";

	}
	catch(CException* e)
	{
		e->GetErrorMessage(message,1024);
		e->Delete();
		m_iCountFailed++;
		strResult = "[FAILED]";

	}
	m_Result.SetItemText(itemPos,1,strResult);
	m_Result.SetItemText(itemPos,2,message);
	DoAppEvents();
	

}

void CTestSettings::TryExecute(CString path)
{
	m_iCountTotal++;

	if(path.IsEmpty())
		return;

	CString strAction;
	strAction.Format("Trying to execute %s",path);
	int itemPos = AddItem(strAction);
	DoAppEvents();
	

	char message[1024];
	ZeroMemory(message,1024);
	CSecurity s;
	CString strResult;


	try
	{
		CRunProcess process;
		HANDLE hToken = NULL;
		DWORD exitCode = process.CreateProcess(hToken, path.GetBuffer(),TRUE);
		if(exitCode==0)
		{
			m_iCountOK++;
			strResult = "[OK]";
		}
		else
		{
			m_iCountFailed++;
			strResult = "[FAILED]";
		}

	}
	catch(CException* e)
	{
		e->GetErrorMessage(message,1024);
		e->Delete();
		m_iCountFailed++;
		strResult = "[FAILED]";

	}
	m_Result.SetItemText(itemPos,1,strResult);
	m_Result.SetItemText(itemPos,2,message);
	DoAppEvents();
	

}


void CTestSettings::TryUserContext(void)
{
	m_iCountTotal++;

	if(CSecret::m_strScriptExecUser.CompareNoCase("SYSTEM")==0 || CSecret::m_strScriptExecUser.IsEmpty())
		return;

	CString strAction;
	strAction.Format("Executing as %s",CSecret::m_strScriptExecUser);
	int itemPos = AddItem(strAction);
	DoAppEvents();
	char message[1024];
	ZeroMemory(message,1024);
	CSecurity s;
	CString strResult;

	HANDLE token;
	try
	{
		s.LogonUser(CSecret::m_strScriptExecUser.GetBuffer(),CSecret::m_strScriptExecPassword.GetBuffer(),token);
		strResult = "[OK]";
		m_iCountOK++;
		RevertToSelf();
	}
	catch(CException* e)
	{
		e->GetErrorMessage(message,1024);
		e->Delete();
		m_iCountFailed++;
		strResult = "[FAILED]";

	}
	m_Result.SetItemText(itemPos,1,strResult);
	m_Result.SetItemText(itemPos,2,message);
	DoAppEvents();

}

void CTestSettings::OnDestroy()
{
	KillTimer(1);
	CDialog::OnDestroy();


	// TODO: Add your message handler code here
}

void CTestSettings::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case 1:
		{
			KillTimer(1);
			TryPing();
			TryConnect(CSecret::m_ServerPingScriptFile);
			TryRead(CSecret::m_ServerPingScriptFile);

			TryUserContext();
			TryConnect(CSecret::m_strScriptFile);

			TryExecute(CSecret::m_strPreAction);
			TryRead(CSecret::m_strScriptFile);
			TryExecute(CSecret::m_strPostAction);
	
			
			TestComplete();
			PrepareLog();
			GetDlgItem(IDC_BUTTON_SAVE_LOG)->EnableWindow();
			GetDlgItem(IDC_BUTTON_SAVE_CLIPBOARD)->EnableWindow();

			

		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}


void CTestSettings::OnBnClickedButtonSaveLog()
{
	CFileDialog  fdlg( FALSE,".txt", "WPKG Client settings report", OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "Text files (*.txt)|*.txt||",  NULL );

	if(fdlg.DoModal()==IDOK)
	{
		SaveLog(fdlg.GetPathName());
		
		
	}
}

int CTestSettings::AddItem(CString action)
{
	return m_Result.InsertItem(m_Result.GetItemCount(),action);;
}

void CTestSettings::TestComplete(void)
{
	
	m_strResultLog.Format("Testing complete\r\n\r\n%u - OK\r\n%u - failed\r\n%u - not tested",
		m_iCountOK,m_iCountFailed,
		m_iCountTotal-m_iCountOK-m_iCountFailed);

	UpdateData(FALSE);
}

void CTestSettings::SaveLog(CString fileName)
{

	CStdioFile f;
	f.Open(fileName,CFile::modeCreate|CFile::modeWrite|CFile::typeText);
	f.WriteString(m_strResultLog);
	f.Close();
}

void CTestSettings::OnBnClickedButtonSaveClipboard()
{
	

	if(OpenClipboard())
	{
		EmptyClipboard();
		HLOCAL hloc = LocalAlloc(LMEM_MOVEABLE,m_strResultLog.GetLength()+1);
		PVOID pBuffer = LocalLock(hloc);
		memcpy(pBuffer,m_strResultLog.GetBuffer(),m_strResultLog.GetLength()+1);
		LocalUnlock(hloc);

		SetClipboardData(CF_TEXT,hloc);
		CloseClipboard();
		
	}
}

void CTestSettings::PrepareLog(void)
{
	m_strResultLog.Remove('\r');

	m_strResultLog += "\n\nDetails:";

	for(int i=0; i< m_Result.GetItemCount(); i++)
	{
		CString str;
		str.Format("\n\nAction: %s\nResult: %s\nMessage: %s",
			m_Result.GetItemText(i,0),
			m_Result.GetItemText(i,1),
			m_Result.GetItemText(i,2));

		m_strResultLog += str;
	}


}
