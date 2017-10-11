#pragma once




class CFileMap
{

protected:
	CTime m_tStartTime;
	CEvent* m_pSyncEvent;
	CMutex* m_pSyncMutex;
	CEvent* m_pLogonDelayWorkingEvent;
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
	LPBYTE m_lpMapAddress;
	CFileMap(void);
	~CFileMap(void);

	BOOL CreateSharedMem();
	BOOL OpenSharedMem();
	void WriteStartDate(CTime date);
	CTime ReadStartDate();
	void WriteMessage(CString str);
	BOOL IsWorking(CString& str, DWORD dwMaxTick);
	void AbortWorkingMonitor();
	void LogonDelayWorking();
	void WaitForLogonDelay();
	
	static BOOL AllowAllAccesSa();
	static BOOL AllowAdminAccesSa();
};

