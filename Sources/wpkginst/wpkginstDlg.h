// wpkginstDlg.h : header file
//

#pragma once

#include "listctrlEx.h"


// CWpkgInstDlg dialog
class CWpkgInstDlg : public CDialog
{
// Construction
public:
	CWpkgInstDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_WPKGINST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	void ShowAdvanced();
	CStringArray m_strVarArray;
	CListCtrlEx m_PathVariables;

	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
	
	CString m_strScriptFile;
	CString m_strScriptParameters;
	CString m_strScriptConnUser;
	CString m_strScriptConnPassword;
	CString m_strScriptExecUser;
	CString m_strScriptExecPassword;

	afx_msg void OnBnClickedButtonHelp();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSelectFile();
	afx_msg void OnBnClickedButtonPathvariableNew();
	afx_msg void OnBnClickedButtonPathvariableDelete();


	void AddScriptVarData(CStringArray& data);
	void GetScriptVarData(CStringArray& data);
	CString m_strPreAction;
	CString m_strPostAction;
	BOOL m_bPreAction;
	BOOL m_bPostAction;
	BOOL m_bShowGUI;

	afx_msg void OnBnClickedCheck();
	afx_msg void OnBnClickedButtonAdvanced();
	afx_msg void OnBnClickedButtonSelectPrefile();
	afx_msg void OnBnClickedButtonSelectPostfile();

	afx_msg void OnBnClickedButtonExportSettings();
};
