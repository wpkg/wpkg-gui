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
private:


protected:
	BOOL m_bAdvanced;
	void ShowAdvanced();
	
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonHelp();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSelectFile();

	afx_msg void OnBnClickedButtonAdvanced();

	afx_msg void OnBnClickedButtonExportSettings();
	CTabCtrlEx m_TabSettings;
	void ReadLogonDelay(void);
	void SaveLogonDelay(void);
	void SaveLogonMessages(void);
	void ReadLogonMessages(void);
	afx_msg void OnBnClickedButtonAbout();
	afx_msg void OnBnClickedButtonImportSettings();
	afx_msg void OnBnClickedButtonTestSettings();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
