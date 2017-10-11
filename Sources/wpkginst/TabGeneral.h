#pragma once

#include "listctrlEx.h"

// CTabGeneral dialog

class CTabGeneral : public CPropertyPage
{
	DECLARE_DYNAMIC(CTabGeneral)

public:
	void GetScriptVarData();
	CTabGeneral();
	virtual ~CTabGeneral();

// Dialog Data
	enum { IDD = IDD_DIALOG_GENERAL };

protected:
	CStringArray m_strVarArray;
	CListCtrlEx m_PathVariables;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonSelectPrefile();
	afx_msg void OnBnClickedButtonSelectPostfile();
	afx_msg void OnBnClickedButtonPathvariableNew();
	afx_msg void OnBnClickedButtonPathvariableDelete();
	afx_msg void OnBnClickedCheck();


public:
	void AddScriptVarData(CStringArray& data);
	void GetScriptVarData(CStringArray& data);
	virtual BOOL OnInitDialog();
	CListCtrl* GetScriptVarCtrl();
};
