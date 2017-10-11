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
	m_strMessage = _T("Unknown error occured.");

	if (dwErrorCode != ERROR_EXTENDED_ERROR) 
	{
		::FormatMessage( 
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dwErrorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) m_strMessage.GetBufferSetLength(1025),
			1024,
			NULL );
	}
	else
	{
		WNetGetLastError(&dwErrorCode,	// error code
			(LPSTR) m_strMessage.GetBufferSetLength(1025), 
										// buffer for error description 
            1024,						// size of error buffer
            (LPSTR) szProvider,			// buffer for provider name 
            sizeof(szProvider));		// size of name buffer
 
	}

	m_strMessage.ReleaseBuffer();
	m_strMessage.Replace("\r\n","");

}


void CExceptionEx::ThrowError(DWORD Error)
{
	CExceptionEx* exception = new CExceptionEx();
	exception->FormatMessage(Error);
	exception->m_dwError = Error;
	throw exception;
}


