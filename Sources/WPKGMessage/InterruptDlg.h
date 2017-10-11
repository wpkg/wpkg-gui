#pragma once


// CInterruptDlg dialog

class CInterruptDlg : public CDialog
{
	DECLARE_DYNAMIC(CInterruptDlg)

public:
	CInterruptDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInterruptDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_INTERRUPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPwd;
};
