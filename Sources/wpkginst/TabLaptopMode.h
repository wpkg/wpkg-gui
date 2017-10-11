#pragma once
#include "afxcmn.h"


// CTabLaptopMode dialog

class CTabLaptopMode : public CPropertyPage
{
	DECLARE_DYNAMIC(CTabLaptopMode)

public:
	CTabLaptopMode();
	virtual ~CTabLaptopMode();

// Dialog Data
	enum { IDD = IDD_DIALOG_LAPTOP_MODE };

protected:
	void EnableEditActions();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSelectFile();
	virtual BOOL OnInitDialog();
	CSpinButtonCtrl m_spinServerDelay;
	CSpinButtonCtrl m_spinCustomScriptDelay;
	afx_msg void CheckControls();
private:
	// on/off validation temporary
	BOOL m_bValidate;
public:
	void RefreshData(void);
};
