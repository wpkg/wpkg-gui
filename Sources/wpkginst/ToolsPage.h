#pragma once
#include "BasePropertyPage.h"

// CToolsPage dialog

class CToolsPage : public CBasePropertyPage
{
	DECLARE_DYNAMIC(CToolsPage)

public:
	CToolsPage();
	virtual ~CToolsPage();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_TOOLS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CEdit m_editServiceState;
	CString m_strServerName;
	afx_msg void OnBnClickedButtonServiceStart();
	afx_msg void OnBnClickedButtonServiceStop();
public:
	void SaveMachines(void);
};
