#pragma once


// CPrioritySettings dialog

class CPrioritySettings : public CPropertyPage
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
	int m_iPriority;

};
