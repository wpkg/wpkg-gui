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
	virtual ~CWPKGMessageDlg();	// standard constructor

// Dialog Data
	enum { IDD = IDD_WPKGMESSAGE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	CString m_strMessage1, m_strMessage2;
	CImage m_logoImage;
	void ReadLogonMessages();
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_Message;
	CProgressCtrl m_Progress;
protected:
	BOOL m_bBringToFront;
	
	afx_msg void OnEndSession(BOOL bEnding);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void OnOK();
	virtual void OnCancel();
public:
	void BringToFront(void);
	
	CStatic m_ProgressMessage;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void MoveCtrl();

};
