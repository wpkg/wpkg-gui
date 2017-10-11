// wpkginstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "wpkginstDlg.h"
#include ".\wpkginstdlg.h"
#include "XmlSettings.h"
#include "afxcmn.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4996)

// CAboutDlg dialog used for App About

static DWORD dwStreamOffset = 0;

static DWORD CALLBACK 
LicenseStreamInCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{

	HRSRC h = FindResource(NULL,"IDR_RTF","RTF");
	DWORD size = SizeofResource(NULL,h);
	HGLOBAL hGlobal = LoadResource(NULL,h);
	LPBYTE buffer = (LPBYTE)LockResource(hGlobal);
	
	DWORD copied = min((DWORD)cb,size-dwStreamOffset);
	memcpy(pbBuff,buffer+dwStreamOffset,copied);
	dwStreamOffset += copied;

	*pcb = copied;

	return 0;
}


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_rEdit;
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT, m_rEdit);
}


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CRect rr;
	m_rEdit.GetRect(&rr);
	rr.DeflateRect(10,10,10,10);
	m_rEdit.SetRect(&rr);
	
	m_rEdit.SetOptions(ECOOP_OR, ECO_AUTOWORDSELECTION);
	
	EDITSTREAM es;

	dwStreamOffset = 0;
	es.dwCookie = (DWORD)0;
	es.pfnCallback = LicenseStreamInCallback; 
	m_rEdit.StreamIn(SF_RTF, es);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()







// CWpkgInstDlg dialog

CString CWpkgInstDlg::m_strPreAction;
CString CWpkgInstDlg::m_strPostAction;
BOOL CWpkgInstDlg::m_bPreAction;
BOOL CWpkgInstDlg::m_bPostAction;
BOOL CWpkgInstDlg::m_bShowGUI;
DWORD CWpkgInstDlg::m_dwLogonDelay;

CString CWpkgInstDlg::m_strMessage1;
CString CWpkgInstDlg::m_strMessage2;



CWpkgInstDlg::CWpkgInstDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWpkgInstDlg::IDD, pParent)
	,m_strScriptFile(_T("wpkg.js"))
	,m_strScriptParameters(_T(""))
	,m_strScriptConnUser(_T(""))
	,m_strScriptConnPassword(_T(""))
	,m_strScriptExecUser(_T(""))
	,m_strScriptExecPassword(_T(""))
	
{
	m_strPreAction.Empty();
	m_strPostAction.Empty();
	m_bShowGUI = FALSE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	ReadLogonDelay();
	ReadLogonMessages();

}

void CWpkgInstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_FILE, m_strScriptFile);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PARAMATERS, m_strScriptParameters);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PATH_USER, m_strScriptConnUser);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PATH_PASSWORD, m_strScriptConnPassword);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_EXEC_USER, m_strScriptExecUser);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_EXEC_PASSWORD, m_strScriptExecPassword);
	DDX_Control(pDX, IDC_TAB_ADVANCED, m_TabSettings);
}

BEGIN_MESSAGE_MAP(CWpkgInstDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_HELP, OnBnClickedButtonHelp)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FILE, OnBnClickedButtonSelectFile)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCED, OnBnClickedButtonAdvanced)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_SETTINGS, OnBnClickedButtonExportSettings)
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, OnBnClickedButtonAbout)
END_MESSAGE_MAP()


// CWpkgInstDlg message handlers

BOOL CWpkgInstDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}



	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_TabSettings.InitTabs();
	ShowAdvanced();

	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWpkgInstDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
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
	BOOL bResult = FALSE;
	bResult = UpdateData() && m_TabSettings.UpdateData();

	if(m_strScriptFile.IsEmpty() )
		AfxMessageBox(IDS_FIELD_REQUIRED);
	else
	{
		if(bResult)
			OnOK();
	}
}

void CWpkgInstDlg::OnBnClickedButtonSelectFile()
{
	UpdateData();

	CFileDialog  fdlg( TRUE,NULL, NULL, 0, "Script files (*.js)|*.js||",  NULL );

	if(fdlg.DoModal()==IDOK)
	{
		m_strScriptFile = fdlg.GetPathName();
		UpdateData(FALSE);
	}

}


void CWpkgInstDlg::AddScriptVarData(CStringArray& data)
{
	m_TabSettings.AddScriptVarData(data);
}

void CWpkgInstDlg::GetScriptVarData(CStringArray& data)
{
	m_TabSettings.GetScriptVarData(data);
}

void CWpkgInstDlg::ShowAdvanced()
{
	CRect rr;
	static BOOL bAdvanced = FALSE;
	int offset = bAdvanced ? 315 : -315;


	GetDlgItem(IDC_TAB_ADVANCED)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_TAB_ADVANCED)->EnableWindow(bAdvanced);


	if(bAdvanced)
		GetDlgItem(IDC_BUTTON_ADVANCED)->SetWindowText("<< General");
	else
		GetDlgItem(IDC_BUTTON_ADVANCED)->SetWindowText("Advanced >>");

	GetDlgItem(IDC_BUTTON_ADVANCED)->GetWindowRect(&rr);
	ScreenToClient(&rr);
	rr.OffsetRect(0,offset);
	GetDlgItem(IDC_BUTTON_ADVANCED)->MoveWindow(&rr);

	GetDlgItem(IDC_BUTTON_ABOUT)->GetWindowRect(&rr);
	ScreenToClient(&rr);
	rr.OffsetRect(0,offset);
	GetDlgItem(IDC_BUTTON_ABOUT)->MoveWindow(&rr);

	GetDlgItem(IDC_BUTTON_HELP)->GetWindowRect(&rr);
	ScreenToClient(&rr);
	rr.OffsetRect(0,offset);
	GetDlgItem(IDC_BUTTON_HELP)->MoveWindow(&rr);
	
	GetDlgItem(IDC_BUTTON_EXPORT_SETTINGS)->GetWindowRect(&rr);
	ScreenToClient(&rr);
	rr.OffsetRect(0,offset);
	GetDlgItem(IDC_BUTTON_EXPORT_SETTINGS)->MoveWindow(&rr);

	GetDlgItem(IDOK)->GetWindowRect(&rr);
	ScreenToClient(&rr);
	rr.OffsetRect(0,offset);
	GetDlgItem(IDOK)->MoveWindow(&rr);
	
	GetWindowRect(&rr);
	rr.bottom += offset;
	MoveWindow(&rr);

	CenterWindow();

	bAdvanced = !bAdvanced;

}


void CWpkgInstDlg::OnBnClickedButtonAdvanced()
{
	ShowAdvanced();
}



void CWpkgInstDlg::OnBnClickedButtonExportSettings()
{
	UpdateData();
	m_TabSettings.UpdateData();

	CString xmlFilePath;

	CFileDialog  fdlg( FALSE,"xml", "settings", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Settings files (*.xml)|*.xml||",  NULL );

	if(fdlg.DoModal()!=IDOK)
		return;
	
	xmlFilePath = fdlg.GetPathName();

	try
	{
		CXmlSettings st;
		

		st.CreateInstance();
		st.Init();
		st.WriteParameter("file",m_strScriptFile);
		st.WriteParameter("path-user",m_strScriptConnUser);
		st.WriteParameter("path-password",st.Crypt(m_strScriptConnPassword));
		st.WriteParameter("exec-user",m_strScriptExecUser);
		st.WriteParameter("exec-password",st.Crypt(m_strScriptExecPassword));
		st.WriteParameter("parameters",m_strScriptParameters);
		st.WriteParameter("silent","YES");
		st.WriteParameter("pre-action",m_strPreAction);
		st.WriteParameter("post-action",m_strPostAction);
		st.WriteParameter("show-GUI",m_bShowGUI?"YES":"NO");

		CString tempStr;
		tempStr.Format("%u",m_dwLogonDelay);
		st.WriteParameter("logon-delay",tempStr);
		st.WriteParameter("logon-message-1",m_strMessage1);
		st.WriteParameter("logon-message-2",m_strMessage2);

		CListCtrl* pPathVariables = m_TabSettings.GetScriptVarCtrl();
		for(int i=0; i<pPathVariables->GetItemCount(); i++)
		{
			st.WriteParameterEx(pPathVariables->GetItemText(i,0),
				pPathVariables->GetItemText(i,1));
		}
		
		st.Save(xmlFilePath);
		AfxMessageBox("Export successfully done.");
	}
	catch(_com_error e)
	{
		CString str;
		str.Format("Error occured while writing parameter to settings file:\n%s",e.ErrorMessage());
		AfxMessageBox(str);
	}
	catch(...)
	{
		AfxMessageBox("Unknown error occured while writing parameter to settings file");
	}
}

void CWpkgInstDlg::ReadLogonDelay(void)
{
	m_dwLogonDelay = 0;

	HKEY phkResult;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Notify\\WPKGLogon",
		0,
		KEY_READ,
		&phkResult );

	DWORD cbData = 4;
	DWORD Type;
	
	RegQueryValueEx( phkResult,
		"MaxWait", 0, &Type, (BYTE*)&m_dwLogonDelay, &cbData );

	RegCloseKey(phkResult); 

	m_dwLogonDelay /= 60;


}

void CWpkgInstDlg::SaveLogonDelay(void)
{
	HKEY phkResult;
	DWORD lpdwDisposition = REG_CREATED_NEW_KEY;

	m_dwLogonDelay *= 60;

	RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Notify\\WPKGLogon",
		0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&phkResult,
		&lpdwDisposition);


	RegSetValueEx( phkResult,
		"MaxWait", 0, REG_DWORD, (BYTE*)&m_dwLogonDelay, 4 );

	RegCloseKey(phkResult); 


}

void CWpkgInstDlg::SaveLogonMessages(void)
{
	HKEY phkResult;
	DWORD lpdwDisposition = REG_CREATED_NEW_KEY;

	char* buffer1 = m_strMessage1.GetBuffer();
	char* buffer2 = m_strMessage2.GetBuffer();

	

	RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\WPKG.ORG\\Logon Settings",
		0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&phkResult,
		&lpdwDisposition);


	RegSetValueEx( phkResult,
		"Logon message 1", 0, REG_SZ, (BYTE*)buffer1, m_strMessage1.GetLength()+1 );

	RegSetValueEx( phkResult,
		"Logon message 2", 0, REG_SZ, (BYTE*)buffer2, m_strMessage2.GetLength()+1 );

	RegCloseKey(phkResult); 

	m_strMessage1.ReleaseBuffer();
	m_strMessage2.ReleaseBuffer();

}

void CWpkgInstDlg::ReadLogonMessages(void)
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

void CWpkgInstDlg::OnBnClickedButtonAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

