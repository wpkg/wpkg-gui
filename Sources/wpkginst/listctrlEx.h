
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// listctl.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx window

class CListCtrlEx : public CListCtrl
{
   
// Construction
public:
    CListCtrlEx();

// Attributes
public:

// Operations
public:
	int HitTestEx(CPoint &point, int *col) const;
	CEdit* EditSubLabel( int nItem, int nCol );

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CListCtrlEx)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CListCtrlEx();

	// Generated message map functions
protected:

    //{{AFX_MSG(CListCtrlEx)
    afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
