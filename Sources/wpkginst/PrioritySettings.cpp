// PrioritySettings.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "PrioritySettings.h"
#include ".\prioritysettings.h"
#include "..\components\secret.h"





// CPrioritySettings dialog

IMPLEMENT_DYNAMIC(CPrioritySettings, CPropertyPage)
CPrioritySettings::CPrioritySettings()
	: CBasePropertyPage(CPrioritySettings::IDD)

	, m_bLogToFile(FALSE)
	
{
}

CPrioritySettings::~CPrioritySettings()
{
}

void CPrioritySettings::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PRIORITY, m_cbPriority);
	DDX_Check(pDX, IDC_CHECK_STOP_SERVICE, CSecret::m_bStopServiceAfterDone);
	DDX_Text(pDX, IDC_EDIT_LOG_FILE, CSecret::m_strLogFile);
	DDX_Check(pDX, IDC_CHECK_LOG_TO_FILE, m_bLogToFile);


}


BEGIN_MESSAGE_MAP(CPrioritySettings, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_LOG_FILE, &CPrioritySettings::OnBnClickedButtonSelectLogFile)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_LOG_FILE, &CPrioritySettings::OnBnClickedButtonOpenLogFile)
	ON_BN_CLICKED(IDC_CHECK_LOG_TO_FILE, &CPrioritySettings::OnBnClickedCheckLogToFile)
END_MESSAGE_MAP()


// CPrioritySettings message handlers

BOOL CPrioritySettings::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	
	PriorityToCombo(CSecret::m_dwPriority);
	

	UpdateCheck();

	DataLoaded();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}




DWORD CPrioritySettings::ComboToPriority()
{
	int iPriority = m_cbPriority.GetCurSel();

	DWORD priority = NORMAL_PRIORITY_CLASS;

	switch(iPriority)
	{
	case 0:
		priority = IDLE_PRIORITY_CLASS;
		break;
	case 1:
		priority = BELOW_NORMAL_PRIORITY_CLASS;
		break;
	case 2:
		priority = NORMAL_PRIORITY_CLASS;
		break;
	case 3:
		priority = ABOVE_NORMAL_PRIORITY_CLASS;
		break;
	case 4:
		priority = HIGH_PRIORITY_CLASS;
		break;
	case 5:
		priority = REALTIME_PRIORITY_CLASS;
		break;

	}
	return priority;
}



void CPrioritySettings::PriorityToCombo(DWORD priority)
{
	int iPriority;

	switch(priority)
	{
	case IDLE_PRIORITY_CLASS:
		iPriority = 0;
		break;
	case BELOW_NORMAL_PRIORITY_CLASS:
		iPriority = 1;
		break;
	case NORMAL_PRIORITY_CLASS:
		iPriority = 2;
		break;
	case ABOVE_NORMAL_PRIORITY_CLASS:
		iPriority = 3;
		break;
	case HIGH_PRIORITY_CLASS:
		iPriority = 4;
		break;
	case REALTIME_PRIORITY_CLASS:
		iPriority = 5;
		break;
	default:
		iPriority = 2;
	}

	m_cbPriority.SetCurSel(iPriority);
}

void CPrioritySettings::OnBnClickedButtonSelectLogFile()
{
	UpdateData();

	CFileDialog  fdlg( TRUE,NULL, NULL, 0, "Wpkg log files (*.log; *.txt)|*.log; *.txt|All files (*.*)|*.*||",  NULL );

	if(fdlg.DoModal()==IDOK)
	{
		CSecret::m_strLogFile = fdlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CPrioritySettings::OnBnClickedButtonOpenLogFile()
{
	UpdateData();
	
	if(!CSecret::m_strLogFile.IsEmpty())
		ShellExecute(NULL, _T("open"), CSecret::m_strLogFile, NULL,NULL, SW_SHOW);
}

void CPrioritySettings::EnableEditLog(void)
{
	GetDlgItem(IDC_EDIT_LOG_FILE)->EnableWindow(m_bLogToFile);
	if(!m_bLogToFile)
		GetDlgItem(IDC_EDIT_LOG_FILE)->SetWindowText("");
	GetDlgItem(IDC_BUTTON_SELECT_LOG_FILE)->EnableWindow(m_bLogToFile);
	
}

void CPrioritySettings::OnBnClickedCheckLogToFile()
{
	UpdateData();
	EnableEditLog();
	
}

void CPrioritySettings::UpdateCheck(void)
{
	m_bLogToFile = !CSecret::m_strLogFile.IsEmpty();

	EnableEditLog();

	UpdateData(FALSE);
}

