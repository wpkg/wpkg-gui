#include "StdAfx.h"
#include ".\filemap.h"
#include <aclapi.h>
#include ".\exceptionex.h"


SECURITY_ATTRIBUTES CFileMap::m_sa;
SID_IDENTIFIER_AUTHORITY CFileMap::siaNTAuthority = SECURITY_NT_AUTHORITY;
SECURITY_DESCRIPTOR CFileMap::m_sd; 
PSID CFileMap::m_pSIDAliasAdmins; 
PACL CFileMap::m_pNewDACL;



CFileMap::CFileMap(void)
{
	m_pSyncEvent = NULL;
	m_pSyncMutex = NULL;
	m_pLogonDelayWorkingEvent = NULL;

	m_hMapFile = NULL;;

	m_szMapName = "Global\\WPKG_INFO_DATA";
	m_uiDataSize = 8192;
	m_lpMapAddress = NULL;
	m_pSIDAliasAdmins = NULL; 

	m_tStartTime = 0;
}

CFileMap::~CFileMap(void)
{
	ReleaseSharedMem();
	TRACE0("ReleaseSharedMem()\n");
}





BOOL CFileMap::CreateSharedMem()
{
	BOOL bResult = FALSE;
	m_lpMapAddress = NULL;
	

	if(!m_pSyncEvent)
		m_pSyncEvent = new CEvent(FALSE,FALSE,"Global\\WPKG_INFO_EVENT",&m_sa);
	
	if(!m_pSyncMutex)
		m_pSyncMutex = new CMutex(FALSE,"Global\\WPKG_INFO_MUTEX",&m_sa);
	

	if(!m_pLogonDelayWorkingEvent)
		m_pLogonDelayWorkingEvent = new CEvent(FALSE,FALSE,"Global\\WPKG_LOGON_DELAY_WORKING_EVENT",&m_sa);

	
	ASSERT(m_pSyncEvent);
	ASSERT(m_pSyncMutex);
	ASSERT(m_pLogonDelayWorkingEvent);


	if((m_hMapFile = CreateFileMapping((HANDLE)-1, &m_sa,
		PAGE_READWRITE, NULL, m_uiDataSize, m_szMapName)) != NULL)
		bResult = TRUE;


	if(bResult)
		m_lpMapAddress = (LPBYTE)MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	if(m_lpMapAddress)
	{
		ZeroMemory(m_lpMapAddress,m_uiDataSize);
		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	if(!bResult)
	{
		CExceptionEx::ThrowError("CreateSharedMem",GetLastError());
	}

	return bResult;

}

BOOL CFileMap::OpenSharedMem()
{
	BOOL bResult = FALSE;
	m_lpMapAddress = NULL;

	if(!m_pSyncEvent)
		m_pSyncEvent = new CEvent(FALSE,FALSE,"Global\\WPKG_INFO_EVENT",&m_sa);
	if(!m_pSyncMutex)
		m_pSyncMutex = new CMutex(FALSE,"Global\\WPKG_INFO_MUTEX",&m_sa);

	if(!m_pLogonDelayWorkingEvent)
		m_pLogonDelayWorkingEvent = new CEvent(FALSE,FALSE,"Global\\WPKG_LOGON_DELAY_WORKING_EVENT",&m_sa);


	m_hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE,
		m_szMapName);

	if (m_hMapFile != NULL) 
		bResult = TRUE;

	if(bResult)
		m_lpMapAddress = (LPBYTE)MapViewOfFile(m_hMapFile, FILE_MAP_READ, 0, 0, 0);

	if(m_lpMapAddress)
	{
		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	if(!bResult)
	{
		CExceptionEx::ThrowError("OpenSharedMem",GetLastError());
	}

	return bResult;

}


void CFileMap::WriteStartDate(CTime date)
{
	if(m_tStartTime==0)
		m_tStartTime = date;

	if(!m_lpMapAddress)
		return;

	m_pSyncMutex->Lock();
	ZeroMemory(m_lpMapAddress,sizeof(CTime));
	memcpy((char*)m_lpMapAddress,&date,sizeof(CTime));
	m_pSyncMutex->Unlock();
	Sleep(5);

	m_pSyncEvent->SetEvent();
}

CTime CFileMap::ReadStartDate()
{
	if(!m_lpMapAddress)
		return CTime::GetCurrentTime();

	m_pSyncMutex->Lock();
	CTime date;
	memcpy(&date,(char*)m_lpMapAddress,sizeof(CTime));
	m_pSyncMutex->Unlock();
	Sleep(5);

	return date;
}

void CFileMap::WriteMessage(CString str)
{
	m_pSyncMutex->Lock();

	ZeroMemory(m_lpMapAddress+sizeof(CTime),m_uiDataSize-sizeof(CTime));
	memcpy((char*)m_lpMapAddress+sizeof(CTime),str.GetBuffer(),min(str.GetLength()+1,m_uiDataSize-sizeof(CTime)));

	m_pSyncMutex->Unlock();
	Sleep(5);

	m_pSyncEvent->SetEvent();
}

void CFileMap::AbortWorkingMonitor()
{
	m_pSyncEvent->SetEvent();

}
BOOL CFileMap::IsWorking(CString& str, DWORD dwMaxTick)
{
	DWORD tick = GetTickCount();

	BOOL bWorking = FALSE;

	if(tick>=dwMaxTick)
		return bWorking;

	CSingleLock sl(m_pSyncEvent);
	if(sl.Lock(dwMaxTick-tick))
	{

		m_pSyncMutex->Lock();

		DWORDLONG date;
		memcpy(&date,(char*)m_lpMapAddress,sizeof(CTime));
		bWorking = (date!=0);


		memcpy(str.GetBufferSetLength(m_uiDataSize-sizeof(CTime)),(char*)m_lpMapAddress+sizeof(CTime),m_uiDataSize-sizeof(CTime));
		str.ReleaseBuffer();

		m_pSyncMutex->Unlock();

		sl.Unlock();
		Sleep(5);
	}

	return bWorking;
}

void CFileMap::WaitForLogonDelay()
{
	OSVERSIONINFO osi;
	osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &osi );

	if(osi.dwMajorVersion>5)
		return;

	DWORD dwTimeOut = 0; // in seconds


	HKEY phkResult;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\Notify\\WPKGLogon",
		0,
		KEY_READ,
		&phkResult );
	
	DWORD cbData = sizeof(DWORD);
	DWORD Type;
	
	// in registry timeout is in seconds
	RegQueryValueEx( phkResult,
		"MaxWait", 0, &Type, (BYTE*)&dwTimeOut, &cbData );

	RegCloseKey(phkResult); 


	// logon delay must be longer then wpkgmessage wait
	if(dwTimeOut >= 90)
		dwTimeOut -= 30;

	dwTimeOut*=1000;

	CTimeSpan ts = CTime::GetCurrentTime() - m_tStartTime;
	LONGLONG elapsed = ts.GetTotalSeconds();
	elapsed*=1000;

	if(elapsed<dwTimeOut)
		dwTimeOut-=elapsed;
	else
		dwTimeOut = 0;

	
	CSingleLock sl(m_pLogonDelayWorkingEvent);
	sl.Lock(dwTimeOut);
	
}

void CFileMap::LogonDelayWorking()
{
	m_pLogonDelayWorkingEvent->SetEvent();
}


void CFileMap::ReleaseSharedMem()
{
	if( m_lpMapAddress )
	{
		UnmapViewOfFile(m_lpMapAddress);
		m_lpMapAddress = NULL;
	}
	if( m_hMapFile )
	{
		CloseHandle(m_hMapFile); 
		m_hMapFile = NULL; 
	}

	if(m_pSyncEvent)
	{
		delete m_pSyncEvent;
		m_pSyncEvent = NULL;
	}
	if(m_pSyncMutex)
	{
		delete m_pSyncMutex;
		m_pSyncMutex = NULL;
	}

	if(m_pLogonDelayWorkingEvent)
	{
		delete m_pLogonDelayWorkingEvent;
		m_pLogonDelayWorkingEvent = NULL;
	}

}

BOOL CFileMap::AllowAllAccesSa()
{
	ZeroMemory( &m_sd, sizeof( m_sd ) );
	ZeroMemory( &m_sa, sizeof( m_sa ) );

	m_sa.nLength = sizeof( SECURITY_ATTRIBUTES ); 
	m_sa.lpSecurityDescriptor = &m_sd;
	m_sa.bInheritHandle = TRUE; 


	// Initialize a security descriptor and assign it a NULL 
	// discretionary ACL to allow unrestricted access. 


	if (!InitializeSecurityDescriptor(&m_sd, 
		SECURITY_DESCRIPTOR_REVISION)) 
	{ 
		return FALSE; 
	} 
	if (!SetSecurityDescriptorDacl(&m_sd, 
		TRUE, 
		(PACL) NULL, 
		FALSE)) 
	{
		return FALSE; 
	}
	return TRUE;
}	

BOOL CFileMap::AllowAdminAccesSa()
{
	ZeroMemory( &m_sd, sizeof( m_sd ) );
	ZeroMemory( &m_sa, sizeof( m_sa ) );

	m_sa.nLength = sizeof( SECURITY_ATTRIBUTES ); 
	m_sa.lpSecurityDescriptor = &m_sd;
	m_sa.bInheritHandle = TRUE; 

	EXPLICIT_ACCESS ea[ 2 ];

	if (!InitializeSecurityDescriptor(&m_sd, 
		SECURITY_DESCRIPTOR_REVISION)) 
	{ 
		return FALSE; 
	} 


	if (!AllocateAndInitializeSid(&siaNTAuthority, 
		2,                     /* subauthority count  */ 
		/* first subauthority */ 
		SECURITY_BUILTIN_DOMAIN_RID, 
		/* second subauthority */ 
		DOMAIN_ALIAS_RID_ADMINS, 
		//DOMAIN_GROUP_RID_ADMINS,
		0, 0, 0, 0, 0, 0, 
		&m_pSIDAliasAdmins)) 
	{ 

		return FALSE; 
	} 

	ZeroMemory( ea, sizeof( EXPLICIT_ACCESS ) * 2 );
	ea[0].grfAccessPermissions = GENERIC_ALL;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = NO_INHERITANCE;
	ea[0].Trustee.pMultipleTrustee = NULL;
	ea[0].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[0].Trustee.ptstrName = 	(char*)m_pSIDAliasAdmins;

	char* System = "SYSTEM"; 


	BuildExplicitAccessWithName( ea + 1, System, GENERIC_ALL, 
		SET_ACCESS, NO_INHERITANCE);

	SetEntriesInAcl( 2, ea, NULL, &m_pNewDACL);

	if (!SetSecurityDescriptorDacl(&m_sd, 
		TRUE, 
		(PACL) m_pNewDACL, 
		FALSE)) 
	{ 

		return FALSE; 
	}

	return TRUE;
}