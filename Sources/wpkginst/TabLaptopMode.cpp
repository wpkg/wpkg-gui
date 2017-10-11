// TabLaptopMode.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "TabLaptopMode.h"
#include ".\tablaptopmode.h"
#include "..\components\secret.h"


// CTabLaptopMode dialog

void AFXAPI DDV_ServerTimeOut(
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
				"Server connecting timeout - must be in range from 3 to 10 secods",
				"Warning",
				MB_ICONWARNING);
			
			pDX->Fail();
		
		}
	}
}

void AFXAPI DDV_ScriptTimeOut(
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
				"Script execution timeout - must be in range from 3 to 60 secods",
				"Warning",
				MB_ICONWARNING);
			pDX->Fail();
		}
	}
}

void AFXAPI DDV_EmptyStringServer(
                            CDataExchange* pDX,
							CString value, BOOL offline, int testMethod)
   
{
	if(pDX->m_bSaveAndValidate)
	{
			
		if(value.IsEmpty() && offline && testMethod == 0)
		
		{
			MessageBox(pDX->m_pDlgWnd->GetSafeHwnd(),
				"This value could not be empty",
				"Warning",
				MB_ICONWARNING);
			pDX->Fail();
		}
	}
}

void AFXAPI DDV_EmptyStringCustomScript(
                            CDataExchange* pDX,
							CString value, BOOL offline, int testMethod)
   
{
	if(pDX->m_bSaveAndValidate)
	{
		
		if(value.IsEmpty() && offline && testMethod == 1)
		
		{
			MessageBox(pDX->m_pDlgWnd->GetSafeHwnd(),
				"This value could not be empty",
				"Warning",
				MB_ICONWARNING);
			pDX->Fail();
		}
	}
}
   


IMPLEMENT_DYNAMIC(CTabLaptopMode, CPropertyPage)
CTabLaptopMode::CTabLaptopMode()
	: CBasePropertyPage(CTabLaptopMode::IDD)
	, m_bValidate(TRUE)
{
}

CTabLaptopMode::~CTabLaptopMode()
{
}

void CTabLaptopMode::DoDataExchange(CDataExchange* pDX)
{

	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_ENABLE_LAPTOP_MODE, CSecret::m_bLaptopMode);
	DDX_Radio(pDX, IDC_RADIO_SERVER, CSecret::m_iServerTestingMethod);

	DDX_Text(pDX, IDC_EDIT_SERVER, CSecret::m_strServerIP);
	if(m_bValidate)
		DDV_EmptyStringServer(pDX, CSecret::m_strServerIP,CSecret::m_bLaptopMode,CSecret::m_iServerTestingMethod);
	DDX_Text(pDX, IDC_EDIT_CUSTOM_SCRIPT, CSecret::m_ServerPingScriptFile);
	if(m_bValidate)
		DDV_EmptyStringCustomScript(pDX, CSecret::m_ServerPingScriptFile,CSecret::m_bLaptopMode,CSecret::m_iServerTestingMethod);

	DDX_Text(pDX, IDC_EDIT_SERVER_MAX_DELAY, CSecret::m_dwServerPingTimeout);
	if(m_bValidate)
		DDV_ServerTimeOut(pDX, CSecret::m_dwServerPingTimeout, 3, 10);
	DDX_Text(pDX, IDC_EDIT_CUSTOM_SCRIPT_MAX_DELAY, CSecret::m_dwServerPingScriptTimeout);
	if(m_bValidate)
		DDV_ScriptTimeOut(pDX, CSecret::m_dwServerPingScriptTimeout, 3, 60);
	DDX_Control(pDX, IDC_SPIN_SERVER_MAX_DELAY, m_spinServerDelay);
	DDX_Control(pDX, IDC_SPIN_CUSTOM_SCRIPT_MAX_DELAY, m_spinCustomScriptDelay);
	

	
}


BEGIN_MESSAGE_MAP(CTabLaptopMode, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FILE, OnBnClickedButtonSelectFile)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_LAPTOP_MODE, CheckControls)
	ON_BN_CLICKED(IDC_RADIO_SERVER, CheckControls)
	ON_BN_CLICKED(IDC_RADIO_CUSTOM_SCRIPT, CheckControls)
END_MESSAGE_MAP()


// CTabLaptopMode message handlers

void CTabLaptopMode::OnBnClickedButtonSelectFile()
{
	RefreshData();

	CFileDialog  fdlg( TRUE,NULL, NULL, 0, "Executable files (*.exe; *.bat; *com)|*.exe; *.bat; *.com|All files (*.*)|*.*||",  NULL );

	if(fdlg.DoModal()==IDOK)
	{
		CSecret::m_ServerPingScriptFile = fdlg.GetPathName();
		UpdateData(FALSE);
	}

}

BOOL CTabLaptopMode::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	m_spinServerDelay.SetRange(3,10);
	m_spinServerDelay.SetBuddy(GetDlgItem(IDC_EDIT_SERVER_MAX_DELAY));

	m_spinCustomScriptDelay.SetRange(3,60);
	m_spinCustomScriptDelay.SetBuddy(GetDlgItem(IDC_EDIT_CUSTOM_SCRIPT_MAX_DELAY));

	EnableEditActions();

	DataLoaded();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CTabLaptopMode::EnableEditActions()
{

	GetDlgItem(IDC_RADIO_CUSTOM_SCRIPT)->EnableWindow(CSecret::m_bLaptopMode);
	GetDlgItem(IDC_RADIO_SERVER)->EnableWindow(CSecret::m_bLaptopMode);

	GetDlgItem(IDC_EDIT_SERVER)->EnableWindow(CSecret::m_iServerTestingMethod==0&&CSecret::m_bLaptopMode);
	GetDlgItem(IDC_EDIT_SERVER_MAX_DELAY)->EnableWindow(CSecret::m_iServerTestingMethod==0&&CSecret::m_bLaptopMode);
	GetDlgItem(IDC_SPIN_SERVER_MAX_DELAY)->EnableWindow(CSecret::m_iServerTestingMethod==0&&CSecret::m_bLaptopMode);
	if(CSecret::m_iServerTestingMethod!=0)
		GetDlgItem(IDC_EDIT_SERVER)->SetWindowText("");

	GetDlgItem(IDC_EDIT_CUSTOM_SCRIPT)->EnableWindow(CSecret::m_iServerTestingMethod==1&&CSecret::m_bLaptopMode);
	GetDlgItem(IDC_BUTTON_SELECT_FILE)->EnableWindow(CSecret::m_iServerTestingMethod==1&&CSecret::m_bLaptopMode);
	GetDlgItem(IDC_EDIT_CUSTOM_SCRIPT_MAX_DELAY)->EnableWindow(CSecret::m_iServerTestingMethod==1&&CSecret::m_bLaptopMode);
	GetDlgItem(IDC_SPIN_CUSTOM_SCRIPT_MAX_DELAY)->EnableWindow(CSecret::m_iServerTestingMethod==1&&CSecret::m_bLaptopMode);
    if(CSecret::m_iServerTestingMethod!=1)
		GetDlgItem(IDC_EDIT_CUSTOM_SCRIPT)->SetWindowText("");

	if(!CSecret::m_bLaptopMode)
	{
		GetDlgItem(IDC_EDIT_SERVER)->SetWindowText("");
		GetDlgItem(IDC_EDIT_CUSTOM_SCRIPT)->SetWindowText("");
	}
	
}

void CTabLaptopMode::CheckControls()
{
	RefreshData();
	EnableEditActions();
}


void CTabLaptopMode::RefreshData(void)
{
	m_bValidate=FALSE;
	UpdateData();
	m_bValidate=TRUE;
}
