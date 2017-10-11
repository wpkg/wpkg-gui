#pragma once

#include "TabGeneral.h"
#include "TabLogonSettings.h"
#include "PrioritySettings.h"


// CTabCtrlEx

class CTabCtrlEx : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabCtrlEx)

public:
	CTabCtrlEx();
	virtual ~CTabCtrlEx();
	CTabGeneral m_dlg1;
	CTabLogonSettings m_dlg2;
	CPrioritySettings m_dlg3;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
	void InitTabs(void);
	BOOL UpdateData(void);
	void AddScriptVarData(CStringArray& data);
	void GetScriptVarData(CStringArray& data);
	CListCtrl* GetScriptVarCtrl();
	DWORD GetPriority();
	void SetPriority(DWORD priority);
	CString GetXmlPriority();

};


