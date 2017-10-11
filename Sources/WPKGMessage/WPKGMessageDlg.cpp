// WPKGMessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WPKGMessage.h"
#include "WPKGMessageDlg.h"
#include ".\wpkgmessagedlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CWPKGMessageDlg::CWPKGMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWPKGMessageDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	ReadLogonMessages();
}

void CWPKGMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_Message);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
}

BEGIN_MESSAGE_MAP(CWPKGMessageDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWPKGMessageDlg message handlers

BOOL CWPKGMessageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	SetTimer(1,10000,NULL);
	SetTimer(2,1000,NULL);

	m_Progress.SetRange(0,10);
	m_Progress.SetStep(1);

	m_Message.SetWindowText(m_strMessage1);

	BringToFront();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWPKGMessageDlg::OnPaint() 
{
	CDialog::OnPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWPKGMessageDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWPKGMessageDlg::OnTimer(UINT nIDEvent)
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
	CRect rr;
	GetWindowRect(&rr);
	SetWindowPos(&wndTopMost ,rr.left,rr.top,rr.Width(),rr.Height(),SWP_SHOWWINDOW);
	CenterWindow();
}


void CWPKGMessageDlg::ReadLogonMessages(void)
{
	m_strMessage1 = "WPKG is installing applications and applying settings...";
	m_strMessage2 = "Please wait, don't restart or power off your computer...";

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

}
