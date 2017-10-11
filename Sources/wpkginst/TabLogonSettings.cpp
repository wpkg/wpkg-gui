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


CTabLogonSettings::CTabLogonSettings()
	: CPropertyPage(CTabLogonSettings::IDD)
	
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
	DDX_Text(pDX, IDC_EDIT_LOGON_MESSAGE1, CSecret::m_strMessage1);
	DDV_MaxChars(pDX, CSecret::m_strMessage1, 80);
	DDX_Text(pDX, IDC_EDIT_LOGON_MESSAGE2, CSecret::m_strMessage2);
	DDV_MaxChars(pDX, CSecret::m_strMessage2, 80);
	DDX_Control(pDX, IDC_SPIN_LOGON_DELAY, m_spinLogonDelay);
	DDX_Control(pDX, IDC_STATIC_VISTA_PROMPT, m_stVistaPrompt);
}


BEGIN_MESSAGE_MAP(CTabLogonSettings, CPropertyPage)
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
		GetDlgItem(IDC_EDIT_LOGON_MESSAGE1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LOGON_MESSAGE2)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_VISTA_PROMPT)->EnableWindow();
		GetDlgItem(IDC_STATIC_VISTA_PROMPT)->ShowWindow(SW_SHOW);
	}


}


BOOL CTabLogonSettings::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	m_spinLogonDelay.SetRange(0,120);
	m_spinLogonDelay.SetBuddy(GetDlgItem(IDC_EDIT_LOGON_DELAY));

	DisableLogonDelaying();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
