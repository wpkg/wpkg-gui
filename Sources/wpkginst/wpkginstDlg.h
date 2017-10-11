// wpkginstDlg.h : header file
//

#pragma once


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
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	CString m_strScriptPath;
	CString m_strScriptFile;
	CString m_strScriptParameters;
	CString m_strScriptConnUser;
	CString m_strScriptConnPassword;
	CString m_strScriptExecUser;
	CString m_strScriptExecPassword;

	afx_msg void OnBnClickedButtonHelp();
	CString m_strScriptVarValue1;
	CString m_strScriptVarName1;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSelectFile();
};
