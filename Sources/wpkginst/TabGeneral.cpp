// TabGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "TabGeneral.h"
#include "wpkginstDlg.h"
#include ".\tabgeneral.h"
#include "..\components\secret.h"


// CTabGeneral dialog

IMPLEMENT_DYNAMIC(CTabGeneral, CPropertyPage)
CTabGeneral::CTabGeneral()
	: CPropertyPage(CTabGeneral::IDD)
{
	m_bPreAction = FALSE;
	m_bPostAction = FALSE;
}

CTabGeneral::~CTabGeneral()
{
}

void CTabGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SCRIPT_VARIABLES, m_PathVariables);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PREACTION, CSecret::m_strPreAction);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_POSTACTION, CSecret::m_strPostAction);
	DDX_Check(pDX, IDC_CHECK_PRE, m_bPreAction);
	DDX_Check(pDX, IDC_CHECK_POST, m_bPostAction);
	DDX_Check(pDX, IDC_CHECK_SHOW_GUI, CSecret::m_bShowGUI);

}


BEGIN_MESSAGE_MAP(CTabGeneral, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_SCRIPTVARIABLE_NEW, OnBnClickedButtonPathvariableNew)
	ON_BN_CLICKED(IDC_BUTTON_SCRIPTVARIABLE_DELETE, OnBnClickedButtonPathvariableDelete)
	ON_BN_CLICKED(IDC_CHECK_PRE, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_POST, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_PREFILE, OnBnClickedButtonSelectPrefile)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_POSTFILE, OnBnClickedButtonSelectPostfile)
END_MESSAGE_MAP()


// CTabGeneral message handlers

void CTabGeneral::OnBnClickedButtonPathvariableNew()
{
	int count = m_PathVariables.GetItemCount();
	count = m_PathVariables.InsertItem(count,"enter name...");
	m_PathVariables.SetItemText(count,1,"enter value...");
	m_PathVariables.EditSubLabel(count,0);
}


void CTabGeneral::OnBnClickedButtonPathvariableDelete()
{
	int nItem = -1;
	nItem = m_PathVariables.GetNextItem(nItem,LVNI_SELECTED);
	if(nItem!=-1)
		m_PathVariables.DeleteItem(nItem);
}


void CTabGeneral::OnBnClickedCheck()
{
	UpdateData();

	EnableEditActions();

}

void CTabGeneral::OnBnClickedButtonSelectPrefile()
{
	UpdateData();

	CFileDialog  fdlg( TRUE,NULL, NULL, 0, "Executable files (*.exe; *.bat; *com)|*.exe; *.bat; *.com|All files (*.*)|*.*||",  NULL );

	if(fdlg.DoModal()==IDOK)
	{
		CSecret::m_strPreAction = fdlg.GetPathName();
		UpdateData(FALSE);
	}

}

void CTabGeneral::OnBnClickedButtonSelectPostfile()
{
	UpdateData();

	CFileDialog  fdlg( TRUE,NULL, NULL, 0, "Executable files (*.exe; *.bat; *com)|*.exe; *.bat; *.com|All files (*.*)|*.*||",  NULL );

	if(fdlg.DoModal()==IDOK)
	{
		CSecret::m_strPostAction = fdlg.GetPathName();
		UpdateData(FALSE);
	}
}

BOOL CTabGeneral::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_PathVariables.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_INFOTIP|
		LVS_EX_GRIDLINES);
	m_PathVariables.InsertColumn(0,"Name",LVCFMT_LEFT,250);
	m_PathVariables.InsertColumn(1,"Value",LVCFMT_LEFT,250);
	
	SetScriptVarData();

	UpdateCheck();

	OnBnClickedCheck();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTabGeneral::SetScriptVarData()
{
	int pos = 0;

	m_PathVariables.DeleteAllItems();

	for(int i=0;i<CSecret::m_strVarArray.GetCount();i+=2)
	{
		pos = m_PathVariables.InsertItem(pos,CSecret::m_strVarArray.GetAt(i));
		m_PathVariables.SetItemText(pos,1,CSecret::m_strVarArray.GetAt(i+1));
		pos++;
	}

}

void CTabGeneral::GetScriptVarData()
{
	CSecret::m_strVarArray.RemoveAll();

	for(int i=0;i<m_PathVariables.GetItemCount();i++)
	{
		CSecret::m_strVarArray.Add(m_PathVariables.GetItemText(i,0));
		CSecret::m_strVarArray.Add(m_PathVariables.GetItemText(i,1));
	}

}

void CTabGeneral::EnableEditActions()
{
	GetDlgItem(IDC_EDIT_SCRIPT_PREACTION)->EnableWindow(m_bPreAction);
	if(!m_bPreAction)
		GetDlgItem(IDC_EDIT_SCRIPT_PREACTION)->SetWindowText("");
	GetDlgItem(IDC_EDIT_SCRIPT_POSTACTION)->EnableWindow(m_bPostAction);
	if(!m_bPostAction)
		GetDlgItem(IDC_EDIT_SCRIPT_POSTACTION)->SetWindowText("");
	GetDlgItem(IDC_BUTTON_SELECT_PREFILE)->EnableWindow(m_bPreAction);
	GetDlgItem(IDC_BUTTON_SELECT_POSTFILE)->EnableWindow(m_bPostAction);
}

void CTabGeneral::UpdateCheck()
{
	m_bPreAction = !CSecret::m_strPreAction.IsEmpty();
	m_bPostAction = !CSecret::m_strPostAction.IsEmpty();

	EnableEditActions();

	UpdateData(FALSE);
}




