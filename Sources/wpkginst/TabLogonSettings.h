#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CTabLogonSettings dialog

class CTabLogonSettings : public CPropertyPage
{
	DECLARE_DYNAMIC(CTabLogonSettings)

public:
	CTabLogonSettings();
	virtual ~CTabLogonSettings();

// Dialog Data
	enum { IDD = IDD_DIALOG_LOGON_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	
	

	CSpinButtonCtrl m_spinLogonDelay;
	virtual BOOL OnInitDialog();
	CStatic m_stVistaPrompt;
private:
	void DisableLogonDelaying();
};
