#pragma once

class CSettings
{
public:
	CSettings(void);
	virtual ~CSettings(void);
	static CString GetAppPath(void);
	static void LoadMachines(CString& strMachines);
	static void SaveMachines(CString strMachines);
	static void SaveLogo(CString strLogo);
	static CString GetLogoPath();
};
