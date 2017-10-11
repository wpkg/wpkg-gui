// BasePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "BasePropertyPage.h"

void AFXAPI DDV_RangeValidate(
   CDataExchange* pDX,
   unsigned value,
   unsigned minVal,
   unsigned maxVal,
   char* message 
)
{
	if(pDX->m_bSaveAndValidate)
	{
		if(value<minVal || value>maxVal)
		{
			MessageBox(pDX->m_pDlgWnd->GetSafeHwnd(),
				message,
				"Warning",
				MB_ICONWARNING);
			pDX->Fail();
		}
	}
}

// CBasePropertyPage dialog

IMPLEMENT_DYNAMIC(CBasePropertyPage, CPropertyPage)

CBasePropertyPage::CBasePropertyPage(UINT idTemplate)
: CPropertyPage(idTemplate)
{
	m_bWindowInitialized = FALSE;
}

CBasePropertyPage::~CBasePropertyPage()
{
}

void CBasePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasePropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// CBasePropertyPage message handlers

LRESULT CBasePropertyPage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case WM_COMMAND:

		switch(HIWORD(wParam))
		{
		case BN_CLICKED:
			if( LOWORD(wParam)==IDC_BUTTON_SERVICE_STOP || LOWORD(wParam)==IDC_BUTTON_SERVICE_START )
				break;
		case EN_CHANGE:
		case CBN_SELCHANGE:
			if(m_bWindowInitialized && LOWORD(wParam)!=IDC_EDIT_SERVICE_STATE)
				SetModified();
			break;

		}
		break;
	}

	return CPropertyPage::WindowProc(message, wParam, lParam);
}

void CBasePropertyPage::DataLoaded(void)
{
	m_bWindowInitialized = TRUE;
}
