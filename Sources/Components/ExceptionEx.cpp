#include "StdAfx.h"
#include ".\exceptionex.h"

IMPLEMENT_DYNAMIC(CExceptionEx, CException)

CExceptionEx::CExceptionEx(void)
{
	m_dwError = 0;
}


CExceptionEx::~CExceptionEx(void)
{
}



BOOL CExceptionEx::GetErrorMessage(LPTSTR lpszError, UINT nMaxError,
	PUINT pnHelpContext)
{
	ASSERT(lpszError != NULL && AfxIsValidString(lpszError, nMaxError));

	if (lpszError == NULL || nMaxError == 0) 
	{
		return FALSE;
	}

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;
	
	lstrcpyn(lpszError, m_strMessage, nMaxError);

	return TRUE;
}

void CExceptionEx::FormatMessage(DWORD dwErrorCode)
{
	CHAR szProvider[256]; 
	m_strSystemMessage = _T("Unknown error occured.");

	if (dwErrorCode != ERROR_EXTENDED_ERROR) 
	{
		::FormatMessage( 
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dwErrorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) m_strSystemMessage.GetBufferSetLength(1025),
			1024,
			NULL );
	}
	else
	{
		WNetGetLastError(&dwErrorCode,	// error code
			(LPSTR) m_strSystemMessage.GetBufferSetLength(1025), 
										// buffer for error description 
            1024,						// size of error buffer
            (LPSTR) szProvider,			// buffer for provider name 
            sizeof(szProvider));		// size of name buffer
 
	}

	m_strSystemMessage.ReleaseBuffer();
	m_strSystemMessage.Replace("\r\n","");

	CString str;
	str.Format("%s-> %s",m_strMessage,m_strSystemMessage);
	m_strMessage = str;


}


void CExceptionEx::ThrowError(CString messageFrom, DWORD Error)
{
	CExceptionEx* exception = new CExceptionEx();
	exception->m_strMessage = messageFrom;
	exception->FormatMessage(Error);
	exception->m_dwError = Error;
	throw exception;
}

void CExceptionEx::ThrowError(CString strError)
{
	CExceptionEx* exception = new CExceptionEx();
	exception->m_strMessage = strError;
	exception->m_dwError = 1;
	throw exception;
}


