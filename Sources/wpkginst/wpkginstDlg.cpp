// wpkginstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "wpkginstDlg.h"
#include ".\wpkginstdlg.h"
#include "..\components\XmlSettings.h"
#include "..\components\secret.h"
#include "..\components\ServiceControl.h"
#include "afxcmn.h"
#include "TestSettings.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4996)

// CAboutDlg dialog used for App About

static DWORD dwStreamOffset = 0;

static DWORD CALLBACK 
LicenseStreamInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
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
	es.dwCookie = (DWORD_PTR)0;
	es.pfnCallback = LicenseStreamInCallback; 
	m_rEdit.StreamIn(SF_RTF, es);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()



// CWpkgInstDlg dialog



CWpkgInstDlg::CWpkgInstDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWpkgInstDlg::IDD, pParent)

{
	//m_bAdvanced = FALSE;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//ReadLogonDelay();
	//ReadLogonMessages();
}

void CWpkgInstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_ADVANCED, m_TabSettings);
}

BEGIN_MESSAGE_MAP(CWpkgInstDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_HELP, OnBnClickedButtonHelp)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_SETTINGS, OnBnClickedButtonExportSettings)
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, OnBnClickedButtonAbout)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_SETTINGS, OnBnClickedButtonImportSettings)
	ON_BN_CLICKED(IDC_BUTTON_TEST_SETTINGS, OnBnClickedButtonTestSettings)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CWpkgInstDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_DONATE, &CWpkgInstDlg::OnBnClickedButtonDonate)
	ON_BN_CLICKED(IDOK, &CWpkgInstDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CWpkgInstDlg::OnBnClickedCancel)
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
	/*
	CRegKey key;
	char value[1024];
	DWORD size = 1024;
	key.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\WPKG.ORG\\Settings",KEY_READ);
	key.QueryValue(value,"Path",&size);
	key.Close();

	CString str(value);
	if(str.Right(1)!='\\')
	str+='\\';

	str += "help.txt";

	ShellExecute(NULL, _T("open"), str, NULL,NULL, SW_SHOW);
	*/
	HINSTANCE result = ShellExecute(NULL, _T("open"),
		"http://wpkg.org/WPKG_Client_-_GUI_help", NULL,NULL, SW_SHOW);

}



void CWpkgInstDlg::OnBnClickedButtonImportSettings()
{
	CString xmlFilePath;

	CFileDialog  fdlg( TRUE,"xml", "settings", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Settings files (*.xml)|*.xml||",  NULL );

	if(fdlg.DoModal()!=IDOK)
		return;

	xmlFilePath = fdlg.GetPathName();

	try
	{
		CSecret s;
		s.Import(xmlFilePath);
		UpdateData(FALSE);
		m_TabSettings.UpdateData(FALSE);

		/// test
		if(UpdateData() && m_TabSettings.UpdateData()) 
			AfxMessageBox("Import successfully done.");
	}
	catch(_com_error e)
	{
		CString str;
		str.Format("Error occured while reading parameter from settings file:\n%s",e.ErrorMessage());
		AfxMessageBox(str);
	}
	catch(CException* e)
	{
		e->ReportError();
		e->Delete();
	}
	catch(...)
	{
		AfxMessageBox("Unknown error occured while reading parameter from settings file");
	}

}


void CWpkgInstDlg::OnBnClickedButtonExportSettings()
{
	if(UpdateData() &&	m_TabSettings.UpdateData())
	{

		CString xmlFilePath;

		CFileDialog  fdlg( FALSE,"xml", "settings", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"Settings files (*.xml)|*.xml||",  NULL );

		if(fdlg.DoModal()!=IDOK)
			return;

		xmlFilePath = fdlg.GetPathName();

		try
		{
			CSecret s;
			s.Export(xmlFilePath);
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
}

/*
void CWpkgInstDlg::ReadLogonDelay(void)
{
CSecret::m_dwLogonDelay = 0;

HKEY phkResult;
RegOpenKeyEx(HKEY_LOCAL_MACHINE,
"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Notify\\WPKGLogon",
0,
KEY_READ,
&phkResult );

DWORD cbData = 4;
DWORD Type;

RegQueryValueEx( phkResult,
"MaxWait", 0, &Type, (BYTE*)&CSecret::m_dwLogonDelay, &cbData );

RegCloseKey(phkResult); 

// logon delay must be longer then wpkgmessage wait
if(CSecret::m_dwLogonDelay>30)
if(CSecret::m_dwLogonDelay-=30)

CSecret::m_dwLogonDelay /= 60;


}
*/

void CWpkgInstDlg::SaveLogonDelay(void)
{
	HKEY phkResult;
	DWORD lpdwDisposition = REG_CREATED_NEW_KEY;

	DWORD logonDelay = CSecret::m_dwLogonDelay * 60;

	// logon delay must be longer then wpkgmessage wait
	if(logonDelay>0)
		logonDelay += 30; // in seconds

	RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Notify\\WPKGLogon",
		0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&phkResult,
		&lpdwDisposition);


	OSVERSIONINFO osi;
	osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &osi );

	DWORD shDelay = 0;
	shDelay = CSecret::m_dwShutdownDelay * 60 * 1000;
	if(shDelay==0)
		shDelay = 20000;

	if(CSecret::m_bRunOnShutdown)
	{
		
		if(osi.dwMajorVersion>5)
		{
			
			CServiceControl::SetPreShutdownTimeout(shDelay);

		}
		else
		{
			shDelay = CSecret::m_dwShutdownDelay * 60;

			RegSetValueEx( phkResult,
				"MaxWait", 0, REG_DWORD, (BYTE*)&shDelay, sizeof(DWORD) );
		}

	}
	else
	{
		RegSetValueEx( phkResult,
			"MaxWait", 0, REG_DWORD, (BYTE*)&logonDelay, sizeof(DWORD) );
	}

	RegCloseKey(phkResult);

	shDelay = CSecret::m_dwShutdownDelay * 60 * 1000;
	if(shDelay==0)
		shDelay = 20000;

	if(osi.dwMajorVersion<=5)
	{

		RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control",
			0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&phkResult,
			&lpdwDisposition);

		RegSetValueEx( phkResult,
			"WaitToKillServiceTimeout", 0, REG_DWORD, (BYTE*)&shDelay, sizeof(DWORD) );

		RegCloseKey(phkResult);
	}
}

/*
void CWpkgInstDlg::SaveLogonMessages(void)
{
HKEY phkResult;
DWORD lpdwDisposition = REG_CREATED_NEW_KEY;

char* buffer1 = CSecret::m_strMessage1.GetBuffer();
char* buffer2 = CSecret::m_strMessage2.GetBuffer();



RegCreateKeyEx(HKEY_LOCAL_MACHINE,
"SOFTWARE\\WPKG.ORG\\Logon Settings",
0, NULL, REG_OPTION_NON_VOLATILE,
KEY_ALL_ACCESS,
NULL,
&phkResult,
&lpdwDisposition);


RegSetValueEx( phkResult,
"Logon message 1", 0, REG_SZ, (BYTE*)buffer1, CSecret::m_strMessage1.GetLength()+1 );

RegSetValueEx( phkResult,
"Logon message 2", 0, REG_SZ, (BYTE*)buffer2, CSecret::m_strMessage2.GetLength()+1 );

RegCloseKey(phkResult); 

CSecret::m_strMessage1.ReleaseBuffer();
CSecret::m_strMessage2.ReleaseBuffer();

}

void CWpkgInstDlg::ReadLogonMessages(void)
{

HKEY phkResult;
RegOpenKeyEx(HKEY_LOCAL_MACHINE,
"SOFTWARE\\WPKG.ORG\\Logon Settings",
0,
KEY_READ,
&phkResult );

char* buffer1 = CSecret::m_strMessage1.GetBufferSetLength(1024);
char* buffer2 = CSecret::m_strMessage2.GetBufferSetLength(1024);
DWORD cbData = 1024;
DWORD Type;

RegQueryValueEx( phkResult,
"Logon message 1", 0, &Type, (BYTE*)buffer1, &cbData );

cbData = 1024;
RegQueryValueEx( phkResult,
"Logon message 2", 0, &Type, (BYTE*)buffer2, &cbData );


RegCloseKey(phkResult); 

CSecret::m_strMessage1.ReleaseBuffer();
CSecret::m_strMessage2.ReleaseBuffer();

}
*/

void CWpkgInstDlg::OnBnClickedButtonAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}



void CWpkgInstDlg::OnBnClickedButtonTestSettings()
{
	if(UpdateData()&&m_TabSettings.UpdateData())
	{
		CTestSettings dlg;
		dlg.DoModal();
	}

}


BOOL CWpkgInstDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	OnBnClickedButtonHelp();
	return TRUE;
}


LRESULT CWpkgInstDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case PSM_CHANGED:
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow();
		break;
	case PSM_UNCHANGED:
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
		break;
		
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CWpkgInstDlg::OnBnClickedButtonSave()
{
	BOOL bResult = FALSE;
	bResult = UpdateData() && m_TabSettings.UpdateData();
	if(bResult)
	{
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
		CSecret::StoreSecret();
		SaveLogonDelay();
		m_TabSettings.m_dlg5.SaveMachines();
	}
}

void CWpkgInstDlg::OnBnClickedButtonDonate()
{
	HINSTANCE result = ShellExecute(NULL, _T("open"),
		"http://wpkg.org/Donate", NULL,NULL, SW_SHOW);

}


void CWpkgInstDlg::OnBnClickedOk()
{
	if(!PromptToSave())
		OnOK();
}

void CWpkgInstDlg::OnBnClickedCancel()
{
	if(!PromptToSave())
		OnCancel();
}


BOOL CWpkgInstDlg::PromptToSave(void)
{
	BOOL bResult = GetDlgItem(IDC_BUTTON_SAVE)->IsWindowEnabled();
	if(bResult)
	{
		if(AfxMessageBox("You make changes and not save them. Exit without saving?",MB_YESNO|MB_DEFBUTTON2)==IDYES)
			bResult = FALSE;

	}
	return bResult;
}
