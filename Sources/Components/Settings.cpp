#include "StdAfx.h"
#include "Settings.h"

CSettings::CSettings(void)
{
}

CSettings::~CSettings(void)
{
}

CString CSettings::GetAppPath(void)
{
	CRegKey key;
	char value[1024];
	DWORD size = 1024;
	key.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\WPKG.ORG\\Settings",KEY_READ);
	key.QueryValue(value,"Path",&size);
	key.Close();

	return CString(value);
}

void CSettings::LoadMachines(CString& strMachines)
{
	CString str = GetAppPath();

	if(str.Right(1)!='\\')
		str+='\\';
	str += "machines.txt";
	CStdioFile f;
	if(f.Open(str,CFile::modeRead|CFile::typeText))
	{
		ULONGLONG size = f.GetLength();
		char* buffer = strMachines.GetBufferSetLength(size);
		f.Read(buffer,size);
		strMachines.ReleaseBuffer();
		f.Close();
	}
}

void CSettings::SaveMachines(CString strMachines)
{
	CString str = GetAppPath();

	if(str.Right(1)!='\\')
		str+='\\';
	str += "machines.txt";
	CStdioFile f;
	if(f.Open(str,CFile::modeCreate|CFile::modeWrite|CFile::typeText))
	{
		
		char* buffer = strMachines.GetBuffer();
		f.Write(buffer,strMachines.GetLength()+1);
		strMachines.ReleaseBuffer();
		f.Close();
	}
}

void CSettings::SaveLogo(CString strLogo)
{
	CString str = GetAppPath();

	if(str.Right(1)!='\\')
		str+='\\';
	str += "WPKGMessageLogo.png";

	if(strLogo.IsEmpty())
	{
		try
		{
			CFile::Remove(str);
		}
		catch(CException* e)
		{
			e->Delete();
		}

	}
	else
	{
		CImage img;
		img.Load(strLogo);
		img.Save(str,Gdiplus::ImageFormatPNG);
	}
}


CString CSettings::GetLogoPath()
{
	CString str = GetAppPath();

	if(str.Right(1)!='\\')
		str+='\\';
	str += "WPKGMessageLogo.png";

	return str;
}