// Report.h: interface for the CReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORT_H__C006D961_B3D4_11D4_8953_00201880D317__INCLUDED_)
#define AFX_REPORT_H__C006D961_B3D4_11D4_8953_00201880D317__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CReport : public CStdioFile
{
public:
	BOOL m_bFlush;
	CString m_strPointerString;
	void SetSize( ULONGLONG size );
	BOOL AddInfo( CString text,... );
	BOOL OpenReport( CString name );
	CReport();
	virtual ~CReport();

private:
	CCriticalSection m_csWrite;
	ULONGLONG m_MaxLength;
	void Truncate();
};

#endif // !defined(AFX_REPORT_H__C006D961_B3D4_11D4_8953_00201880D317__INCLUDED_)
