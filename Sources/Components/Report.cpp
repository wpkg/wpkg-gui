// Report.cpp: implementation of the CReport class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Report.h"
#include "exceptionex.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReport::CReport()
{
	m_MaxLength = 0xFFFFF;
	m_strPointerString = " -> ";
	m_bFlush = FALSE;
}

CReport::~CReport()
{
	if( m_hFile != INVALID_HANDLE_VALUE )
		Close();
}

BOOL CReport::OpenReport(CString name)
{
	BOOL OK=FALSE;
	m_strFileName = name;
	

	if( m_hFile == INVALID_HANDLE_VALUE )
	{

		OK = Open(name,CFile::modeReadWrite|
			CFile::modeCreate|CFile::modeNoTruncate|
			CFile::shareDenyNone|
			CFile::typeText );

		if(!OK)
		{
			CExceptionEx::ThrowError(m_strFileName, GetLastError());
			return OK;
		}

		SeekToEnd();
		return OK;
	}
	else
		return FALSE;
}

BOOL CReport::AddInfo(CString text,...)
{
	if( m_hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	
	
	try
	{
		CSingleLock sl(&m_csWrite);
		sl.Lock();

		Truncate();

		CTime t = CTime::GetCurrentTime();
		CString result;
		CString str;

		va_list argList;
		va_start( argList, text );
		str.FormatV( text, argList );
		va_end( argList );

		result.Format("%s%s%s\n",t.Format("%Y-%m-%d %H:%M:%S"),m_strPointerString,str);

		printf("%s\n",result);

		WriteString(result);
		if(m_bFlush)
			Flush();

		sl.Unlock();
	}
	catch( CException* e )
	{
		e->Delete();
		return FALSE;
	}
	return TRUE;
}

void CReport::Truncate()
{
	char* buffer = NULL;
	try
	{

		ULONGLONG length = GetLength();
		CString str;
		if( length > m_MaxLength )
		{
			// teraz dzia³a!
			TRACE0("CReport::Truncate. Over length\n");
			ULONGLONG pos;
			pos = Seek( m_MaxLength/10-m_MaxLength,CFile::end);
			ReadString(str);
			pos = GetPosition();
			pos = CFile::Seek( pos,CFile::begin);
			DWORD newlen = DWORD(length - pos);
			buffer = new char[newlen];
			if( buffer )
			{
				UINT bytesRead = CFile::Read( buffer, newlen );
				if( bytesRead == newlen )
				{
					TRACE2("CReport::Truncate. Report truncate() bytesRead=%u, newlen=%u\n",bytesRead,newlen);
					CFile::SeekToBegin();
					CFile::Write( buffer, bytesRead );
					CFile::SetLength( bytesRead );
				}
			}
		}
	}
	catch( CException* e )
	{
		e->Delete();
	}
	if( buffer )
		delete[] buffer;


}

void CReport::SetSize(ULONGLONG size)
{
	m_MaxLength = size;
}

