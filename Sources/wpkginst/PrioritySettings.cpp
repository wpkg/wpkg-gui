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
	: CPropertyPage(CPrioritySettings::IDD)

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
}


BEGIN_MESSAGE_MAP(CPrioritySettings, CPropertyPage)
END_MESSAGE_MAP()


// CPrioritySettings message handlers

BOOL CPrioritySettings::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	
	PriorityToCombo(CSecret::m_dwPriority);

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
