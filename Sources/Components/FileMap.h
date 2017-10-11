#pragma once




class CFileMap
{

protected:
	void ReleaseSharedMem();

	static SID_IDENTIFIER_AUTHORITY 
		siaNTAuthority;

	static SECURITY_DESCRIPTOR m_sd; 
	static PSID m_pSIDAliasAdmins; 
	static PACL m_pNewDACL;


public:

	static SECURITY_ATTRIBUTES m_sa; 

	
	
	HANDLE m_hMapFile;
	CString m_szMapName;
	DWORD m_uiDataSize;
	LPVOID m_lpMapAddress;
	CFileMap(void);
	~CFileMap(void);

	BOOL CreateSharedMem();
	BOOL OpenSharedMem();
	void WriteStartDate(CTime date);
	CTime ReadStartDate();
	static BOOL AllowAllAccesSa();
	static BOOL AllowAdminAccesSa();
};

