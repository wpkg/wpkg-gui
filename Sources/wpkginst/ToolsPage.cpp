// ToolsPage.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "ToolsPage.h"
#include "..\components\ServiceControl.h"
#include "..\components\Settings.h"
#include "SrvCtrlResultDlg.h"


// CToolsPage dialog

IMPLEMENT_DYNAMIC(CToolsPage, CPropertyPage)

CToolsPage::CToolsPage()
	: CBasePropertyPage(CToolsPage::IDD)
	, m_strServerName(_T(""))
{

	CSettings::LoadMachines(m_strServerName);
}

CToolsPage::~CToolsPage()
{

}


BEGIN_MESSAGE_MAP(CToolsPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_SERVICE_START, &CToolsPage::OnBnClickedButtonServiceStart)
	ON_BN_CLICKED(IDC_BUTTON_SERVICE_STOP, &CToolsPage::OnBnClickedButtonServiceStop)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CToolsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVER_NAME, m_strServerName);
	DDX_Control(pDX, IDC_EDIT_SERVICE_STATE, m_editServiceState);
}


// CToolsPage message handlers

void CToolsPage::OnBnClickedButtonServiceStart()
{
	CString strResult;
	UpdateData();

	m_strServerName.Replace(_T("\r\n"),_T("#"));
	CString resToken;
	int curPos = 0;

	resToken= m_strServerName.Tokenize(_T("#"),curPos);
	while (resToken != _T(""))
	{
		strResult += CServiceControl::ServiceControl(resToken,FALSE,&m_editServiceState);
		strResult += _T("\r\n");
		resToken = m_strServerName.Tokenize(_T("#"), curPos);
	};

	CSrvCtrlResultDlg dlg;
	dlg.m_strResult = strResult;
	dlg.DoModal();
	
}

void CToolsPage::OnBnClickedButtonServiceStop()
{
	CString strResult;
	UpdateData();

	m_strServerName.Replace(_T("\r\n"),_T("#"));
	CString resToken;
	int curPos = 0;

	resToken= m_strServerName.Tokenize(_T("#"),curPos);
	while (resToken != _T(""))
	{
		strResult += CServiceControl::ServiceControl(resToken,TRUE,&m_editServiceState);
		strResult += _T("\r\n");
		resToken = m_strServerName.Tokenize(_T("#"), curPos);
	};

	CSrvCtrlResultDlg dlg;
	dlg.m_strResult = strResult;
	dlg.DoModal();
}

BOOL CToolsPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	DataLoaded();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}




void CToolsPage::SaveMachines(void)
{
	CSettings::SaveMachines(m_strServerName);
}
