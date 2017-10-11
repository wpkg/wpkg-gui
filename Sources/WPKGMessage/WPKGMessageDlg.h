// WPKGMessageDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CWPKGMessageDlg dialog
class CWPKGMessageDlg : public CDialog
{
// Construction
public:
	CWPKGMessageDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_WPKGMESSAGE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	CString m_strMessage1, m_strMessage2;
	void ReadLogonMessages();
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	CStatic m_Message;
	CProgressCtrl m_Progress;
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	void BringToFront(void);
	
};
