#pragma once
#include "afxwin.h"
#include "BasePropertyPage.h"


// CPrioritySettings dialog

class CPrioritySettings : public CBasePropertyPage
{
	DECLARE_DYNAMIC(CPrioritySettings)

public:
	CPrioritySettings();
	virtual ~CPrioritySettings();

// Dialog Data
	enum { IDD = IDD_DIALOG_PRIORITY_SETTINGS };

protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void PriorityToCombo(DWORD priority);
	DWORD ComboToPriority();

	
	virtual BOOL OnInitDialog();
private:
	CComboBox m_cbPriority;


public:

	BOOL m_bLogToFile;
	afx_msg void OnBnClickedButtonSelectLogFile();
	afx_msg void OnBnClickedButtonOpenLogFile();
	void EnableEditLog(void);
	afx_msg void OnBnClickedCheckLogToFile();
	void UpdateCheck(void);
	

};
