// wpkginstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "wpkginstDlg.h"
#include ".\wpkginstdlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4996)

// CWpkgInstDlg dialog



CWpkgInstDlg::CWpkgInstDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWpkgInstDlg::IDD, pParent)
	,m_strScriptPath(_T(""))
	,m_strScriptFile(_T("wpkg.js"))
	,m_strScriptParameters(_T(""))
	,m_strScriptConnUser(_T(""))
	,m_strScriptConnPassword(_T(""))
	,m_strScriptExecUser(_T(""))
	,m_strScriptExecPassword(_T(""))
	,m_strScriptVarValue1(_T(""))
	,m_strScriptVarName1(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWpkgInstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PATH, m_strScriptPath);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_FILE, m_strScriptFile);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PARAMATERS, m_strScriptParameters);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PATH_USER, m_strScriptConnUser);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PATH_PASSWORD, m_strScriptConnPassword);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_EXEC_USER, m_strScriptExecUser);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_EXEC_PASSWORD, m_strScriptExecPassword);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_VAR_VALUE1, m_strScriptVarValue1);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_VAR_NAME1, m_strScriptVarName1);
}

BEGIN_MESSAGE_MAP(CWpkgInstDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_HELP, OnBnClickedButtonHelp)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CWpkgInstDlg message handlers

BOOL CWpkgInstDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWpkgInstDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWpkgInstDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CWpkgInstDlg::OnBnClickedButtonHelp()
{
	CRegKey key;
	char value[1024];
	DWORD size = 1024;
	key.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\WPKG.ORG\\Settings",KEY_READ);
	key.QueryValue(value,"Path",&size);
	key.Close();

	CString str(value);
	if(str.Right(1)!='\\')
		str+='\\';

	str += "WPKG Installer Parameters.txt";
	
	ShellExecute(NULL, _T("open"), str, NULL,NULL, SW_SHOW);
}

void CWpkgInstDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if(m_strScriptPath.IsEmpty() || m_strScriptFile.IsEmpty() )
		AfxMessageBox(IDS_FIELD_REQUIRED);
	else
		OnOK();
}
