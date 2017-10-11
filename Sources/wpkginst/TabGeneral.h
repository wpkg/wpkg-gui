#pragma once

#include "listctrlEx.h"

// CTabGeneral dialog

class CTabGeneral : public CPropertyPage
{
	DECLARE_DYNAMIC(CTabGeneral)

public:
	void UpdateCheck();
	void GetScriptVarData();
	void SetScriptVarData();
	CTabGeneral();
	virtual ~CTabGeneral();

// Dialog Data
	enum { IDD = IDD_DIALOG_GENERAL };

protected:
	void EnableEditActions();
	BOOL m_bPreAction;
	BOOL m_bPostAction;

	
	CListCtrlEx m_PathVariables;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonSelectPrefile();
	afx_msg void OnBnClickedButtonSelectPostfile();
	afx_msg void OnBnClickedButtonPathvariableNew();
	afx_msg void OnBnClickedButtonPathvariableDelete();
	afx_msg void OnBnClickedCheck();


public:
	virtual BOOL OnInitDialog();
	
};
