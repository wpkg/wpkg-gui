// TabCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "TabCtrlEx.h"
#include ".\tabctrlex.h"


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
END_MESSAGE_MAP()



// CTabCtrlEx message handlers


void CTabCtrlEx::OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult)
{
	int dlgId = GetCurSel();



	m_dlg1.ShowWindow(SW_HIDE);
	m_dlg2.ShowWindow(SW_HIDE);
	m_dlg3.ShowWindow(SW_HIDE);

	switch(dlgId)
	{
	case 0:
		if(m_dlg2.UpdateData())
			m_dlg1.ShowWindow(SW_SHOW);
		else
		{
			m_dlg2.ShowWindow(SW_SHOW);
			SetCurSel(1);
		}

		break;
	case 1:
		m_dlg2.ShowWindow(SW_SHOW);
		break;
	case 2:
		m_dlg3.ShowWindow(SW_SHOW);
		break;

	}

	*pResult = 0;
}

void CTabCtrlEx::InitTabs(void)
{
	InsertItem(0,"General");
	InsertItem(1,"Logon settings");
	InsertItem(2,"Priority settings");

	m_dlg1.Create(IDD_DIALOG_GENERAL,this);
	m_dlg2.Create(IDD_DIALOG_LOGON_SETTINGS,this);
	m_dlg3.Create(IDD_DIALOG_PRIORITY_SETTINGS,this);

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
	
	m_dlg1.ShowWindow(SW_SHOW);
	m_dlg2.ShowWindow(SW_HIDE);
	m_dlg3.ShowWindow(SW_HIDE);

}

BOOL CTabCtrlEx::UpdateData(void)
{
	BOOL bResult = FALSE;
	bResult = m_dlg1.UpdateData() && m_dlg2.UpdateData() &&
		m_dlg3.UpdateData();

	m_dlg1.GetScriptVarData();
	return bResult;
}


void CTabCtrlEx::AddScriptVarData(CStringArray& data)
{
	m_dlg1.AddScriptVarData(data);
	
}

void CTabCtrlEx::GetScriptVarData(CStringArray& data)
{
	m_dlg1.GetScriptVarData(data);
}

CListCtrl* CTabCtrlEx::GetScriptVarCtrl()
{
	return m_dlg1.GetScriptVarCtrl();
}

void CTabCtrlEx::SetPriority(DWORD priority)
{
	switch(priority)
	{
	case IDLE_PRIORITY_CLASS:
		m_dlg3.m_iPriority = 0;
		break;
	case BELOW_NORMAL_PRIORITY_CLASS:
		m_dlg3.m_iPriority = 1;
		break;
	case NORMAL_PRIORITY_CLASS:
		m_dlg3.m_iPriority = 2;
		break;
	case ABOVE_NORMAL_PRIORITY_CLASS:
		m_dlg3.m_iPriority = 3;
		break;
	case HIGH_PRIORITY_CLASS:
		m_dlg3.m_iPriority = 4;
		break;
	case REALTIME_PRIORITY_CLASS:
		m_dlg3.m_iPriority = 5;
		break;

	}

	
}

CString CTabCtrlEx::GetXmlPriority()
{
	CString priority = "normal";

	switch(m_dlg3.m_iPriority)
	{
	case 0:
		priority = "idle";
		break;
	case 1:
		priority = "below-normal";
		break;
	case 2:
		priority = "normal";
		break;
	case 3:
		priority = "above-normal";
		break;
	case 4:
		priority = "high";
		break;
	case 5:
		priority = "realtime";
		break;
	}
	return priority;
}

DWORD CTabCtrlEx::GetPriority()
{

	DWORD priority = NORMAL_PRIORITY_CLASS;

	switch(m_dlg3.m_iPriority)
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
