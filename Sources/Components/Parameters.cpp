#include "StdAfx.h"
#include ".\parameters.h"

CParameters::CParameters(void)
{
	m_strParDelimiter = "--";
	m_strValDelimiter = '=';
}

CParameters::~CParameters(void)
{
}

void CParameters::SetCommandLine(CString command)
{
	m_strCommandLine = command;
}

void CParameters::Compute(void)
{
	int pos = 0;
	int tempPos;
	CString str;
	m_array.RemoveAll();
	//m_strCommandLine.Remove(' ');

	while(pos > -1)
	{
		tempPos = pos;
		pos = m_strCommandLine.Find(m_strParDelimiter,pos+m_strParDelimiter.GetLength());
		if(pos==-1)
			str = m_strCommandLine.Mid(tempPos+m_strParDelimiter.GetLength());
		else
			str = m_strCommandLine.Mid(tempPos+m_strParDelimiter.GetLength(),pos-tempPos-m_strParDelimiter.GetLength());

		m_array.Add(str);
	}
	

}


CString CParameters::GetParameter(CString paramName)
{
	CString strResult;
	CString line;
	for(int i=0; i<m_array.GetSize(); i++)
	{
		line = m_array.GetAt(i);
		if(GetName(line).CompareNoCase(paramName)==0)
		{
			strResult = GetValue(line);
			break;
		}

	}
	return strResult;
}

CString CParameters::GetValue(CString& line)
{
	CString strResult;
	int pos = line.Find(m_strValDelimiter);
	strResult = line.Mid(pos+m_strValDelimiter.GetLength());
	strResult.Trim();
	return strResult;

}

CString CParameters::GetName(CString& line)
{
	int pos = line.Find(m_strValDelimiter);
	return line.Left(pos);
}

