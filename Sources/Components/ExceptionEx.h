#pragma once
#include "afx.h"

class CExceptionEx :
	public CException
{
	DECLARE_DYNAMIC(CExceptionEx)
private:
	void FormatMessage(DWORD dwErrorCode);

public:
	CString m_strMessage;
	CString m_strSystemMessage;
	DWORD m_dwError;
	CExceptionEx(void);
	virtual ~CExceptionEx(void);
	virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError,
		PUINT pnHelpContext = NULL);

	static void PASCAL ThrowError(CString messageFrom, DWORD Error); 
	static void PASCAL ThrowError(CString strError);


};
