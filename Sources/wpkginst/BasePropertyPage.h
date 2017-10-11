#pragma once

void AFXAPI DDV_RangeValidate(
   CDataExchange* pDX,
   unsigned value,
   unsigned minVal,
   unsigned maxVal,
   char* message 
);


// CBasePropertyPage dialog

class CBasePropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CBasePropertyPage)

public:
	CBasePropertyPage(UINT idTemplate);
	virtual ~CBasePropertyPage();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	
private:
	BOOL m_bWindowInitialized;
public:
	void DataLoaded(void);
};
