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
		m_dlg0.ShowWindow(SW_SHOW);
		break;
	case 1:
		m_dlg1.ShowWindow(SW_SHOW);
		break;
	case 2:
		m_dlg2.ShowWindow(SW_SHOW);
		break;
	case 3:
		m_dlg3.ShowWindow(SW_SHOW);
		break;
	case 4:
		m_dlg4.ShowWindow(SW_SHOW);
		break;
	case 5:
		m_dlg5.ShowWindow(SW_SHOW);
		break;

	}


	*pResult = 0;

}

void CTabCtrlEx::InitTabs(void)
{
	InsertItem(0,"Path, users");
	InsertItem(1,"Variables, actions");
	InsertItem(2,"Logon settings");
	InsertItem(3,"Misc settings");
	InsertItem(4,"Offline mode settings");
	InsertItem(5,"Tools");

	m_dlg0.Create(IDD_DIALOG_GENERAL_MAIN,this);
	m_dlg1.Create(IDD_DIALOG_GENERAL,this);
	m_dlg2.Create(IDD_DIALOG_LOGON_SETTINGS,this);
	m_dlg3.Create(IDD_DIALOG_PRIORITY_SETTINGS,this);
	m_dlg4.Create(IDD_DIALOG_LAPTOP_MODE,this);
	m_dlg5.Create(IDD_DIALOG_TOOLS,this);

	CRect tabRect, itemRect;

	int nX, nY, nXc, nYc;
	GetClientRect(&tabRect);
	GetItemRect(0, &itemRect);

	nX = itemRect.left;
	nY = itemRect.bottom +itemRect.top ;
	nXc=tabRect.Width() - itemRect.left*2;
	nYc=tabRect.Height() - nY - itemRect.top*2;

	m_dlg0.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	m_dlg1.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	m_dlg2.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	m_dlg3.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	m_dlg4.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	m_dlg5.SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);

	HidePages();

	m_dlg0.ShowWindow(SW_SHOW);

}

BOOL CTabCtrlEx::UpdateData(BOOL bUpdate)
{
	BOOL bResult = FALSE;


	if(bUpdate)
	{
		if(!m_dlg0.UpdateData(bUpdate))
		{
			HidePages();
			m_dlg0.ShowWindow(SW_SHOW);
			SetCurSel(0);
			return bResult;
		}

		if(!m_dlg1.UpdateData(bUpdate))
		{
			HidePages();
			m_dlg1.ShowWindow(SW_SHOW);
			SetCurSel(1);
			return bResult;
		}
		if(!m_dlg2.UpdateData(bUpdate))
		{
			HidePages();
			m_dlg2.ShowWindow(SW_SHOW);
			SetCurSel(2);
			return bResult;
		}
		if(!m_dlg3.UpdateData(bUpdate))
		{
			HidePages();
			m_dlg3.ShowWindow(SW_SHOW);
			SetCurSel(3);
			return bResult;
		}
		if(!m_dlg4.UpdateData(bUpdate))
		{
			HidePages();
			m_dlg4.ShowWindow(SW_SHOW);
			SetCurSel(4);
			return bResult;
		}
		if(!m_dlg5.UpdateData(bUpdate))
		{
			HidePages();
			m_dlg5.ShowWindow(SW_SHOW);
			SetCurSel(5);
			return bResult;
		}

		bResult = TRUE;
		CSecret::m_dwPriority = m_dlg3.ComboToPriority();
		CSecret::m_bRunOnShutdown = m_dlg2.ComboToShutdownMode();
		m_dlg1.GetScriptVarData();
	}
	else
	{
		bResult = m_dlg0.UpdateData(bUpdate) && m_dlg1.UpdateData(bUpdate) &&
			m_dlg2.UpdateData(bUpdate) && m_dlg3.UpdateData(bUpdate) &&
			m_dlg4.UpdateData(bUpdate) && m_dlg5.UpdateData(bUpdate);

		m_dlg3.PriorityToCombo(CSecret::m_dwPriority);
		m_dlg2.ShutdownModeToCombo(CSecret::m_bRunOnShutdown);
		m_dlg2.OnCbnSelchangeComboExecutionMode();
		m_dlg3.UpdateCheck();
		m_dlg1.SetScriptVarData();
		m_dlg1.UpdateCheck();
		m_dlg0.OnBnClickedCheckNetUseMachineAccount();
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
	m_dlg0.ShowWindow(SW_HIDE);
	m_dlg1.ShowWindow(SW_HIDE);
	m_dlg2.ShowWindow(SW_HIDE);
	m_dlg3.ShowWindow(SW_HIDE);
	m_dlg4.ShowWindow(SW_HIDE);
	m_dlg5.ShowWindow(SW_HIDE);

}



LRESULT CTabCtrlEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case PSM_CHANGED:
		{
			ASSERT(::IsWindow(m_hWnd));
			ASSERT(GetParent() != NULL);
			CWnd* pParentWnd = GetParent();
			pParentWnd->SendMessage(PSM_CHANGED, (WPARAM)m_hWnd);
		}
		break;
	case PSM_UNCHANGED:
		{
			ASSERT(::IsWindow(m_hWnd));
			ASSERT(GetParent() != NULL);
			CWnd* pParentWnd = GetParent();
			pParentWnd->SendMessage(PSM_UNCHANGED, (WPARAM)m_hWnd);
		}
		break;

	}


	return CTabCtrl::WindowProc(message, wParam, lParam);
}
