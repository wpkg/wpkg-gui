#pragma once


// CSrvCtrlResultDlg dialog

class CSrvCtrlResultDlg : public CDialog
{
	DECLARE_DYNAMIC(CSrvCtrlResultDlg)

public:
	CSrvCtrlResultDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSrvCtrlResultDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SERVICE_CONTROL_RESULT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strResult;
};
