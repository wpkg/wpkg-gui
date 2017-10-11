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
	m_hMapFile = NULL;;
	m_szMapName = "Global\\WPKG_INFO_DATA";
	m_uiDataSize = 4;
	m_lpMapAddress = NULL;
	m_pSIDAliasAdmins = NULL; 
}

CFileMap::~CFileMap(void)
{
	ReleaseSharedMem();
}





BOOL CFileMap::CreateSharedMem()
{
	BOOL bResult = FALSE;
	m_lpMapAddress = NULL;

	if((m_hMapFile = CreateFileMapping((HANDLE)-1, &m_sa,
	  PAGE_READWRITE, NULL, m_uiDataSize, m_szMapName)) != NULL)
		bResult = TRUE;

	if(bResult)
		m_lpMapAddress = MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

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

	m_hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE,
		m_szMapName);
 
	if (m_hMapFile != NULL) 
		bResult = TRUE;
 
	if(bResult)
		m_lpMapAddress = MapViewOfFile(m_hMapFile, FILE_MAP_READ, 0, 0, 0);

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
	if(!m_lpMapAddress)
		return;

	ZeroMemory(m_lpMapAddress,m_uiDataSize);

	memcpy((char*)m_lpMapAddress,&date,m_uiDataSize);
	Sleep(5);
}

CTime CFileMap::ReadStartDate()
{
	if(!m_lpMapAddress)
		return CTime::GetCurrentTime();

	CTime date;
	memcpy(&date,(char*)m_lpMapAddress,m_uiDataSize);
	Sleep(5);

	return date;
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