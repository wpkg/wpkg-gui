// TabGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "TabGeneral.h"
#include "wpkginstDlg.h"
#include ".\tabgeneral.h"


// CTabGeneral dialog

IMPLEMENT_DYNAMIC(CTabGeneral, CPropertyPage)
CTabGeneral::CTabGeneral()
	: CPropertyPage(CTabGeneral::IDD)
{
}

CTabGeneral::~CTabGeneral()
{
}

void CTabGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SCRIPT_VARIABLES, m_PathVariables);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PREACTION, CWpkgInstDlg::m_strPreAction);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_POSTACTION, CWpkgInstDlg::m_strPostAction);
	DDX_Check(pDX, IDC_CHECK_PRE, CWpkgInstDlg::m_bPreAction);
	DDX_Check(pDX, IDC_CHECK_POST, CWpkgInstDlg::m_bPostAction);
	DDX_Check(pDX, IDC_CHECK_SHOW_GUI, CWpkgInstDlg::m_bShowGUI);

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

	GetDlgItem(IDC_EDIT_SCRIPT_PREACTION)->EnableWindow(CWpkgInstDlg::m_bPreAction);
	if(!CWpkgInstDlg::m_bPreAction)
		GetDlgItem(IDC_EDIT_SCRIPT_PREACTION)->SetWindowText("");
	GetDlgItem(IDC_EDIT_SCRIPT_POSTACTION)->EnableWindow(CWpkgInstDlg::m_bPostAction);
	if(!CWpkgInstDlg::m_bPostAction)
		GetDlgItem(IDC_EDIT_SCRIPT_POSTACTION)->SetWindowText("");
	GetDlgItem(IDC_BUTTON_SELECT_PREFILE)->EnableWindow(CWpkgInstDlg::m_bPreAction);
	GetDlgItem(IDC_BUTTON_SELECT_POSTFILE)->EnableWindow(CWpkgInstDlg::m_bPostAction);
}

void CTabGeneral::OnBnClickedButtonSelectPrefile()
{
	UpdateData();

	CFileDialog  fdlg( TRUE,NULL, NULL, 0, "Executable files (*.exe; *.bat; *com)|*.exe; *.bat; *.com|All files (*.*)|*.*||",  NULL );

	if(fdlg.DoModal()==IDOK)
	{
		CWpkgInstDlg::m_strPreAction = fdlg.GetPathName();
		UpdateData(FALSE);
	}

}

void CTabGeneral::OnBnClickedButtonSelectPostfile()
{
	UpdateData();

	CFileDialog  fdlg( TRUE,NULL, NULL, 0, "Executable files (*.exe; *.bat; *com)|*.exe; *.bat; *.com|All files (*.*)|*.*||",  NULL );

	if(fdlg.DoModal()==IDOK)
	{
		CWpkgInstDlg::m_strPostAction = fdlg.GetPathName();
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
	
	int pos = 0;
	for(int i=0;i<m_strVarArray.GetCount();i+=2)
	{
		pos = m_PathVariables.InsertItem(pos,m_strVarArray.GetAt(i));
		m_PathVariables.SetItemText(pos,1,m_strVarArray.GetAt(i+1));
		pos++;
	}


	OnBnClickedCheck();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTabGeneral::GetScriptVarData()
{
	m_strVarArray.RemoveAll();

	for(int i=0;i<m_PathVariables.GetItemCount();i++)
	{
		m_strVarArray.Add(m_PathVariables.GetItemText(i,0));
		m_strVarArray.Add(m_PathVariables.GetItemText(i,1));
	}
}

void CTabGeneral::AddScriptVarData(CStringArray& data)
{
	m_strVarArray.Copy(data);
	
}

void CTabGeneral::GetScriptVarData(CStringArray& data)
{
	data.Copy(m_strVarArray);
}

CListCtrl* CTabGeneral::GetScriptVarCtrl()
{
	return &m_PathVariables;
}


