// WPKGMessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WPKGMessage.h"
#include "WPKGMessageDlg.h"
#include ".\wpkgmessagedlg.h"
#include "..\components\secret.h"
#include "..\components\Settings.h"
#include "InterruptDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const TCHAR szAfxOldWndProc[] = _T("AfxOldWndProc423");



BOOL CALLBACK EnumProc( HWND hWnd, LPARAM lParam)
{
	//check for property and unsubclass if necessary
	WNDPROC oldWndProc = (WNDPROC)::GetProp(hWnd, szAfxOldWndProc);
	if (oldWndProc!=NULL)
	{
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)oldWndProc);
		RemoveProp(hWnd, szAfxOldWndProc);
	}
	return TRUE;
}



CWPKGMessageDlg::CWPKGMessageDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWPKGMessageDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bBringToFront = TRUE;
}

CWPKGMessageDlg::~CWPKGMessageDlg()
{
	WorkingDone();
}



void CWPKGMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_Message);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_STATIC_PROGRESS, m_ProgressMessage);
}

BEGIN_MESSAGE_MAP(CWPKGMessageDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_ENDSESSION()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()


// CWPKGMessageDlg message handlers

BOOL CWPKGMessageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ReadLogonMessages();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	SetTimer(1,10000,NULL);
	SetTimer(2,1000,NULL);

	m_Progress.SetRange(0,10);
	m_Progress.SetStep(1);

	BringToFront();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWPKGMessageDlg::OnPaint() 
{
	CDialog::OnPaint();

	CClientDC dc(this);
	if(!m_logoImage.IsNull())
	{
		m_logoImage.TransparentBlt(dc.m_hDC,10,10,100,100,RGB(255, 0, 255));
		//m_logoImage.Draw(dc.m_hDC,10,10,100,100);
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWPKGMessageDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWPKGMessageDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	static BOOL bMsg = FALSE;

	switch(nIDEvent)
	{
	case 1:
		if(bMsg)
			m_Message.SetWindowText(m_strMessage1);
		else
			m_Message.SetWindowText(m_strMessage2);

		bMsg = !bMsg;

		break;
	case 2:
		{
			if(!IsWorking())
			{
				KillTimer(1);
				KillTimer(2);
				//m_Message.SetWindowText("Software deployment - finished");
				//m_Progress.SetPos(10);
				Sleep(200);
				EndDialog(IDOK);
			}
			else
			{
				int pos = m_Progress.StepIt();
				if(pos >= 10)
					m_Progress.SetPos(0);

				BringToFront();
			}
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CWPKGMessageDlg::OnOK()
{
}

void CWPKGMessageDlg::OnCancel()
{
}

void CWPKGMessageDlg::BringToFront(void)
{
	if(m_bBringToFront)
	{
		CRect rr;
		GetWindowRect(&rr);
		SetWindowPos(&wndTopMost ,rr.left,rr.top,rr.Width(),rr.Height(),SWP_SHOWWINDOW);
		CenterWindow();
	}
}


void CWPKGMessageDlg::ReadLogonMessages(void)
{
	m_strMessage1 = "WPKG is installing applications and applying settings...";
	m_strMessage2 = "Please wait, don't restart or power off your computer...";


	CSecret secret;
	try
	{
		::CoInitialize(NULL);
		secret.LoadSecret();
		m_strMessage1 = secret.m_strMessage1;
		m_strMessage2 = secret.m_strMessage2;

		m_logoImage.Destroy();
		m_logoImage.Load(CSettings::GetLogoPath());
		MoveCtrl();
		
		SetWindowText(secret.m_strMessageTitle);
	}
	catch(CException* e)
	{
		e->Delete();
	}

	::CoUninitialize();


	/*
	HKEY phkResult;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
	"SOFTWARE\\WPKG.ORG\\Logon Settings",
	0,
	KEY_READ,
	&phkResult );

	char* buffer1 = m_strMessage1.GetBufferSetLength(1024);
	char* buffer2 = m_strMessage2.GetBufferSetLength(1024);
	DWORD cbData = 1024;
	DWORD Type;

	RegQueryValueEx( phkResult,
	"Logon message 1", 0, &Type, (BYTE*)buffer1, &cbData );

	cbData = 1024;
	RegQueryValueEx( phkResult,
	"Logon message 2", 0, &Type, (BYTE*)buffer2, &cbData );


	RegCloseKey(phkResult); 

	m_strMessage1.ReleaseBuffer();
	m_strMessage2.ReleaseBuffer();
	*/

}
LRESULT CWPKGMessageDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{				 
	case WM_USER+101:
		{

			char* buffer = (char*)lParam;
			m_ProgressMessage.SetWindowTextA(buffer);
			return 1;
		}
		break;
	case WM_USER+102:
		{
			CInterruptDlg dlg;
			INT_PTR iResult = dlg.DoModal();

			if(iResult == IDOK)
			{
				if(CSecret::m_strInterruptPwd.Compare(dlg.m_strPwd)==0)
				{

					EndDialog(IDOK);
					break;
				}
				else
					AfxMessageBox("Invalid password");

			}
			m_bBringToFront = TRUE;
		}
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}


void CWPKGMessageDlg::OnEndSession(BOOL bEnding)
{
	DWORD dwProcessId;
	DWORD dwThreadId= GetWindowThreadProcessId(m_hWnd,&dwProcessId);
	EnumThreadWindows(dwThreadId, EnumProc,(LPARAM) dwThreadId);
}


BOOL CWPKGMessageDlg::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		// Ctrl+Q
		if(pMsg->wParam == 81 && (GetKeyState(VK_CONTROL) & 0x80000000))
		{
			m_bBringToFront = FALSE;
			PostMessage(WM_USER+102);
		}

		break;
	}
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::PreTranslateMessage(pMsg);
}


void CWPKGMessageDlg::MoveCtrl()
{
	CRect rr;
	if(!m_logoImage.IsNull())
	{
		GetDlgItem(IDC_STATIC_MESSAGE)->GetWindowRect(&rr);
		ScreenToClient(&rr);
		rr.left = 110;
		GetDlgItem(IDC_STATIC_MESSAGE)->MoveWindow(&rr);

		GetDlgItem(IDC_STATIC_PROGRESS)->GetWindowRect(&rr);
		ScreenToClient(&rr);
		rr.left = 110;
		GetDlgItem(IDC_STATIC_PROGRESS)->MoveWindow(&rr);
	}
}
