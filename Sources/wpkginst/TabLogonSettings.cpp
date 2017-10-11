// TabLogonSettings.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "TabLogonSettings.h"
#include "wpkginstDlg.h"
#include ".\tablogonsettings.h"


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
	DDX_Text(pDX, IDC_EDIT_LOGON_DELAY, CWpkgInstDlg::m_dwLogonDelay);
	DDV_LogonDelayMinMaxUnsigned(pDX, CWpkgInstDlg::m_dwLogonDelay, 0, 120);
	DDX_Text(pDX, IDC_EDIT_LOGON_MESSAGE1, CWpkgInstDlg::m_strMessage1);
	DDV_MaxChars(pDX, CWpkgInstDlg::m_strMessage1, 80);
	DDX_Text(pDX, IDC_EDIT_LOGON_MESSAGE2, CWpkgInstDlg::m_strMessage2);
	DDV_MaxChars(pDX, CWpkgInstDlg::m_strMessage2, 80);
	DDX_Control(pDX, IDC_SPIN_LOGON_DELAY, m_spinLogonDelay);
}


BEGIN_MESSAGE_MAP(CTabLogonSettings, CPropertyPage)
END_MESSAGE_MAP()


// CTabLogonSettings message handlers


BOOL CTabLogonSettings::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	m_spinLogonDelay.SetRange(0,120);
	m_spinLogonDelay.SetBuddy(GetDlgItem(IDC_EDIT_LOGON_DELAY));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
