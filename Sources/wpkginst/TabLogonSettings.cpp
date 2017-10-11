// TabLogonSettings.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "TabLogonSettings.h"
#include "wpkginstDlg.h"
#include ".\tablogonsettings.h"
#include "..\components\secret.h"


// CTabLogonSettings dialog

IMPLEMENT_DYNAMIC(CTabLogonSettings, CPropertyPage)

void AFXAPI DDV_LogonDelayMinMaxUnsigned(
   CDataExchange* pDX,
   unsigned value,
   unsigned minVal,
   unsigned maxVal 
)
{
	if(pDX->m_bSaveAndValidate)
	{
		if(value<minVal || value>maxVal)
		{
			MessageBox(pDX->m_pDlgWnd->GetSafeHwnd(),
				"Maximum logon delay - must be in range from 0 to 120 minutes",
				"Warning",
				MB_ICONWARNING);
			pDX->Fail();
		}
	}
}

void AFXAPI DDV_ShutdownDelayMinMaxUnsigned(
   CDataExchange* pDX,
   unsigned value,
   unsigned minVal,
   unsigned maxVal 
)
{
	if(pDX->m_bSaveAndValidate)
	{
		if(value<minVal || value>maxVal)
		{
			MessageBox(pDX->m_pDlgWnd->GetSafeHwnd(),
				"Maximum shutdown delay - must be in range from 1 to 120 minutes",
				"Warning",
				MB_ICONWARNING);
			pDX->Fail();
		}
	}
}



CTabLogonSettings::CTabLogonSettings()
	: CBasePropertyPage(CTabLogonSettings::IDD)
	
{
	
}

CTabLogonSettings::~CTabLogonSettings()
{
}

void CTabLogonSettings::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_LOGON_DELAY, CSecret::m_dwLogonDelay);
	DDV_LogonDelayMinMaxUnsigned(pDX, CSecret::m_dwLogonDelay, 0, 120);

	DDX_Text(pDX, IDC_EDIT_LOGON_MESSAGE_TITLE, CSecret::m_strMessageTitle);
	DDV_MaxChars(pDX, CSecret::m_strMessageTitle, 80);

	CString strLogo = "company logo.png";
	DDX_Text(pDX, IDC_EDIT_LOGON_MESSAGE_LOGO, CSecret::m_strMessageLogo);
	
	DDX_Text(pDX, IDC_EDIT_LOGON_MESSAGE1, CSecret::m_strMessage1);
	DDV_MaxChars(pDX, CSecret::m_strMessage1, 80);

	DDX_Text(pDX, IDC_EDIT_LOGON_MESSAGE2, CSecret::m_strMessage2);
	DDV_MaxChars(pDX, CSecret::m_strMessage2, 80);

	DDX_Control(pDX, IDC_SPIN_LOGON_DELAY, m_spinLogonDelay);
	DDX_Control(pDX, IDC_STATIC_VISTA_PROMPT, m_stVistaPrompt);


	DDX_Control(pDX, IDC_COMBO_EXECUTION_MODE, m_cbShutdownMode);
	DDX_Text(pDX, IDC_EDIT_SHUTDOWN_DELAY, CSecret::m_dwShutdownDelay);
	DDX_Control(pDX, IDC_SPIN_SHUTDOWN_DELAY, m_spinShutdownDelay);
	DDV_ShutdownDelayMinMaxUnsigned(pDX, CSecret::m_dwShutdownDelay, 1, 120);

	DDX_Text(pDX, IDC_EDIT_INTERRUPT_PWD, CSecret::m_strInterruptPwd);
	DDV_MaxChars(pDX, CSecret::m_strInterruptPwd, 20);
	
}


BEGIN_MESSAGE_MAP(CTabLogonSettings, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_EXECUTION_MODE, &CTabLogonSettings::OnCbnSelchangeComboExecutionMode)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FILE, &CTabLogonSettings::OnBnClickedButtonSelectFile)
END_MESSAGE_MAP()


// CTabLogonSettings message handlers

void CTabLogonSettings::DisableLogonDelaying()
{
	OSVERSIONINFO osi;
	osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL OK = GetVersionEx( &osi );

	
	if(osi.dwMajorVersion>5)
	{
		GetDlgItem(IDC_EDIT_LOGON_DELAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LOGON_MESSAGE_TITLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LOGON_MESSAGE_LOGO)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LOGON_MESSAGE1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LOGON_MESSAGE2)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_VISTA_PROMPT)->EnableWindow();
		GetDlgItem(IDC_STATIC_VISTA_PROMPT)->ShowWindow(SW_SHOW);
	}


}

void CTabLogonSettings::EnableLogonTimeout(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_LOGON_DELAY)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPIN_LOGON_DELAY)->EnableWindow(bEnable);
	
}

void CTabLogonSettings::EnableShutdownTimeout(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_SHUTDOWN_DELAY)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPIN_SHUTDOWN_DELAY)->EnableWindow(bEnable);
	
}



BOOL CTabLogonSettings::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	m_spinLogonDelay.SetRange(0,120);
	m_spinLogonDelay.SetBuddy(GetDlgItem(IDC_EDIT_LOGON_DELAY));

	m_spinShutdownDelay.SetRange(1,120);
	m_spinShutdownDelay.SetBuddy(GetDlgItem(IDC_EDIT_SHUTDOWN_DELAY));


	ShutdownModeToCombo(CSecret::m_bRunOnShutdown);


	OnCbnSelchangeComboExecutionMode();

	DisableLogonDelaying();

	DataLoaded();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTabLogonSettings::ShutdownModeToCombo(BOOL mode)
{
	m_cbShutdownMode.SetCurSel(mode?1:0);
}
BOOL CTabLogonSettings::ComboToShutdownMode()
{
	int iMode = m_cbShutdownMode.GetCurSel();
	switch(iMode)
	{
	case 0:
		return FALSE;
		
	case 1:
		return TRUE;
	default:
		return FALSE;
	}

}

void CTabLogonSettings::OnCbnSelchangeComboExecutionMode()
{
	EnableLogonTimeout(!ComboToShutdownMode());
	EnableShutdownTimeout(ComboToShutdownMode());
	DisableLogonDelaying();
}

void CTabLogonSettings::OnBnClickedButtonSelectFile()
{
	UpdateData();

	CFileDialog  fdlg( TRUE,NULL, NULL, 0, "Picture files (*.bmp; *.jpg; *.png; *.gif)|*.bmp; *.jpg; *.png; *.gif|All files (*.*)|*.*||",  NULL );

	if(fdlg.DoModal()==IDOK)
	{
		CSecret::m_strMessageLogo = fdlg.GetPathName();
		UpdateData(FALSE);
	}


}
