// wpkginstDlg.h : header file
//

#pragma once

//#include "listctrlEx.h"

#include "TabCtrlEx.h"
#include "afxcmn.h"



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
	
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
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


	void AddScriptVarData(CStringArray& data);
	void GetScriptVarData(CStringArray& data);


	static CString m_strPreAction;
	static CString m_strPostAction;
	static BOOL m_bPreAction;
	static BOOL m_bPostAction;
	static BOOL m_bShowGUI;
	// Logon delay in minutes
	static DWORD m_dwLogonDelay;

	static CString m_strMessage1;
	static CString m_strMessage2;


	afx_msg void OnBnClickedButtonAdvanced();

	afx_msg void OnBnClickedButtonExportSettings();
	CTabCtrlEx m_TabSettings;
	void ReadLogonDelay(void);
	void SaveLogonDelay(void);
	void SaveLogonMessages(void);
	void ReadLogonMessages(void);
	afx_msg void OnBnClickedButtonAbout();
};
