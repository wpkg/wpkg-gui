#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "BasePropertyPage.h"


// CTabLogonSettings dialog

class CTabLogonSettings : public CBasePropertyPage
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
	void ShutdownModeToCombo(BOOL mode);
	BOOL ComboToShutdownMode();

	

	CSpinButtonCtrl m_spinLogonDelay;
	virtual BOOL OnInitDialog();
	CStatic m_stVistaPrompt;
private:
	void EnableLogonTimeout(BOOL bEnable);
	void EnableShutdownTimeout(BOOL bEnable);

	CComboBox m_cbShutdownMode;
	CSpinButtonCtrl m_spinShutdownDelay;

	void DisableLogonDelaying();
public:
	afx_msg void OnCbnSelchangeComboExecutionMode();
	afx_msg void OnBnClickedButtonSelectFile();
};
