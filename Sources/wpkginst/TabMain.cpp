// PageMain.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "TabMain.h"
#include "..\components\secret.h"

void AFXAPI DDV_EmptyScriptFile(
                            CDataExchange* pDX,
							CString value)
   
{
	if(pDX->m_bSaveAndValidate)
	{
		if(value.IsEmpty())
		{
			CString str;
			str.LoadString(IDS_FIELD_REQUIRED);
			MessageBox(pDX->m_pDlgWnd->GetSafeHwnd(),
				str,
				"Warning",
				MB_ICONWARNING);
			pDX->Fail();
		}
	}
}


// CTabMain dialog

IMPLEMENT_DYNAMIC(CTabMain, CPropertyPage)

CTabMain::CTabMain()
	: CBasePropertyPage(CTabMain::IDD)
{

}

CTabMain::~CTabMain()
{
}

void CTabMain::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_FILE, CSecret::m_strScriptFile);
	DDV_EmptyScriptFile(pDX,CSecret::m_strScriptFile);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PARAMATERS, CSecret::m_strScriptParameters);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PATH_USER, CSecret::m_strScriptConnUser);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PATH_PASSWORD, CSecret::m_strScriptConnPassword);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_EXEC_USER, CSecret::m_strScriptExecUser);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_EXEC_PASSWORD, CSecret::m_strScriptExecPassword);
	DDX_Check(pDX, IDC_CHECK_NET_USE_MACHINE_ACCOUNT, CSecret::m_bNetUseMachineAccount);
	
}


BEGIN_MESSAGE_MAP(CTabMain, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FILE, OnBnClickedButtonSelectFile)
	ON_BN_CLICKED(IDC_CHECK_NET_USE_MACHINE_ACCOUNT, &CTabMain::OnBnClickedCheckNetUseMachineAccount)
END_MESSAGE_MAP()


// CTabMain message handlers

BOOL CTabMain::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	OnBnClickedCheckNetUseMachineAccount();

	DataLoaded();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTabMain::OnBnClickedButtonSelectFile()
{
	UpdateData();

	CFileDialog  fdlg( TRUE,NULL, NULL, 0, "Script files (*.js)|*.js||",  NULL );

	if(fdlg.DoModal()==IDOK)
	{
		CSecret::m_strScriptFile = fdlg.GetPathName();
		UpdateData(FALSE);
	}

}
void CTabMain::OnBnClickedCheckNetUseMachineAccount()
{
	UpdateData();

	GetDlgItem(IDC_EDIT_SCRIPT_PATH_USER)->EnableWindow(!CSecret::m_bNetUseMachineAccount);
	GetDlgItem(IDC_EDIT_SCRIPT_PATH_PASSWORD)->EnableWindow(!CSecret::m_bNetUseMachineAccount);
}
