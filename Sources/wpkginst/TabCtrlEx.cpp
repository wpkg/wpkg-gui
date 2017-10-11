// TabCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "TabCtrlEx.h"
#include ".\tabctrlex.h"
#include "..\components\secret.h"


// CTabCtrlEx

IMPLEMENT_DYNAMIC(CTabCtrlEx, CTabCtrl)
CTabCtrlEx::CTabCtrlEx()
{
}

CTabCtrlEx::~CTabCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CTabCtrlEx, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnTcnSelchange)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnTcnSelchanging)
END_MESSAGE_MAP()



// CTabCtrlEx message handlers


void CTabCtrlEx::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	int dlgId = GetCurSel();

	HidePages();

	switch(dlgId)
	{
	case 0:
		m_dlg1.ShowWindow(SW_SHOW);
		break;
	case 1:
		m_dlg2.ShowWindow(SW_SHOW);
		break;
	case 2:
		m_dlg3.ShowWindow(SW_SHOW);
		break;
	case 3:
		m_dlg4.ShowWindow(SW_SHOW);
		break;

	}


	*pResult = 0;

}

void CTabCtrlEx::InitTabs(void)
{
	InsertItem(0,"General");
	InsertItem(1,"Logon settings");
	InsertItem(2,"Misc settings");
	InsertItem(3,"Offline mode settings");

	m_dlg1.Create(IDD_DIALOG_GENERAL,this);
	m_dlg2.Create(IDD_DIALOG_LOGON_SETTINGS,this);
	m_dlg3.Create(IDD_DIALOG_PRIORITY_SETTINGS,this);
	m_dlg4.Create(IDD_DIALOG_LAPTOP_MODE,this);

	CRect tabRect, itemRect;

	int nX, nY, nXc, nYc;
	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	nX = itemRect.left;
	nY = itemRect.bottom +itemRect.top ;
	nXc=tabRect.Width() - itemRect.left*2;
	nYc=tabRect.Height() - nY - itemRect.top*2;

	m_dlg1.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	m_dlg2.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	m_dlg3.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	m_dlg4.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);

	HidePages();

	m_dlg1.ShowWindow(SW_SHOW);

}

BOOL CTabCtrlEx::UpdateData(BOOL bUpdate)
{
	BOOL bResult = FALSE;


	if(bUpdate)
	{


		if(!m_dlg1.UpdateData(bUpdate))
		{
			ShowAdvanced();
			m_dlg1.ShowWindow(SW_SHOW);
			SetCurSel(0);
			return bResult;
		}
		if(!m_dlg2.UpdateData(bUpdate))
		{
			ShowAdvanced();
			m_dlg2.ShowWindow(SW_SHOW);
			SetCurSel(1);
			return bResult;
		}
		if(!m_dlg3.UpdateData(bUpdate))
		{
			ShowAdvanced();
			m_dlg3.ShowWindow(SW_SHOW);
			SetCurSel(2);
			return bResult;
		}
		if(!m_dlg4.UpdateData(bUpdate))
		{
			ShowAdvanced();
			m_dlg4.ShowWindow(SW_SHOW);
			SetCurSel(3);
			return bResult;
		}

		bResult = TRUE;
		CSecret::m_dwPriority = m_dlg3.ComboToPriority();
		m_dlg1.GetScriptVarData();
	}
	else
	{
		bResult = m_dlg1.UpdateData(bUpdate) && m_dlg2.UpdateData(bUpdate) && m_dlg3.UpdateData(bUpdate) &&
			m_dlg4.UpdateData(bUpdate);

		m_dlg3.PriorityToCombo(CSecret::m_dwPriority);
		m_dlg1.SetScriptVarData();
		m_dlg1.UpdateCheck();
		m_dlg4.CheckControls();
	}


	return bResult;
}



void CTabCtrlEx::OnTcnSelchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = !UpdateData(TRUE);
}

void CTabCtrlEx::HidePages(void)
{
	m_dlg1.ShowWindow(SW_HIDE);
	m_dlg2.ShowWindow(SW_HIDE);
	m_dlg3.ShowWindow(SW_HIDE);
	m_dlg4.ShowWindow(SW_HIDE);

}

void CTabCtrlEx::ShowAdvanced(void)
{
	HidePages();
	CWnd* pParent = GetParent();
	if(pParent)
		::SendMessage(pParent->GetSafeHwnd(),WMU_SHOW_ADVANCED,0,0);

}
