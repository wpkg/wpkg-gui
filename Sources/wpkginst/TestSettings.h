#pragma once
#include "afxcmn.h"


// CTestSettings dialog

class CTestSettings : public CDialog
{
	DECLARE_DYNAMIC(CTestSettings)

public:
	CTestSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestSettings();

// Dialog Data
	enum { IDD = IDD_DIALOG_TEST_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	
	
private:
	virtual BOOL OnInitDialog();
	CListCtrl m_Result;
	void DoAppEvents(void);
	void TryRead(CString path);
	void TryExecute(CString path);
	void TryUserContext(void);
	void TryPing();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	void TryConnect(CString path);
	CString m_strResultLog;
	afx_msg void OnBnClickedButtonSaveLog();
	int AddItem(CString action);
	void TestComplete(void);
	int m_iCountOK;
	int m_iCountFailed;
	int m_iCountTotal;
	void SaveLog(CString fileName);
	afx_msg void OnBnClickedButtonSaveClipboard();
	void PrepareLog(void);
};
