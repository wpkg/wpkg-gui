
#pragma once


#include "TabGeneral.h"
#include "TabLogonSettings.h"
#include "PrioritySettings.h"
#include "TabLaptopMode.h"

#define WMU_SHOW_ADVANCED WM_USER+101 



// CTabCtrlEx

class CTabCtrlEx : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabCtrlEx)

public:
	CTabCtrlEx();
	virtual ~CTabCtrlEx();
	CTabGeneral			m_dlg1;
	CTabLogonSettings	m_dlg2;
	CPrioritySettings	m_dlg3;
	CTabLaptopMode		m_dlg4;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
	void InitTabs(void);
	BOOL UpdateData(BOOL bUpdate=TRUE);
	

	afx_msg void OnTcnSelchanging(NMHDR *pNMHDR, LRESULT *pResult);
	void HidePages(void);
	void ShowAdvanced(void);
};


