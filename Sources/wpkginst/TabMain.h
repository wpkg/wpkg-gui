#pragma once

#include "BasePropertyPage.h"


// CTabMain dialog

class CTabMain : public CBasePropertyPage
{
	DECLARE_DYNAMIC(CTabMain)

public:
	CTabMain();
	virtual ~CTabMain();

// Dialog Data
	enum { IDD = IDD_DIALOG_GENERAL_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	afx_msg void OnBnClickedButtonSelectFile();
public:
	afx_msg void OnBnClickedCheckNetUseMachineAccount();
};
