// PrioritySettings.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "PrioritySettings.h"


// CPrioritySettings dialog

IMPLEMENT_DYNAMIC(CPrioritySettings, CPropertyPage)
CPrioritySettings::CPrioritySettings()
	: CPropertyPage(CPrioritySettings::IDD)
	, m_iPriority(0)
{
}

CPrioritySettings::~CPrioritySettings()
{
}

void CPrioritySettings::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_IDLE_PRIORITY_CLASS, m_iPriority);
}


BEGIN_MESSAGE_MAP(CPrioritySettings, CPropertyPage)
END_MESSAGE_MAP()


// CPrioritySettings message handlers
