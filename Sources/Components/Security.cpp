#include "StdAfx.h"
#include ".\security.h"
#include <aclapi.h>
#include "lmaccess.h"
#include "lmapibuf.h"
#include "exceptionex.h"


SECURITY_ATTRIBUTES CSecurity::m_sa;


CSecurity::CSecurity(void)
{
	m_pSIDAliasAdmins = NULL; 
}

CSecurity::~CSecurity(void)
{
}


BOOL CSecurity::AllowAllAccesSa()
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

BOOL CSecurity::AllowAdminAccesSa()
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

BOOL CSecurity::IsAdmin()
{
	HANDLE hToken;
	DWORD  dwStatus;
	DWORD  dwAccessMask;
	DWORD  dwAccessDesired;
	DWORD  dwACLSize;
	DWORD  dwStructureSize = sizeof(PRIVILEGE_SET);
	PACL   pACL            = NULL;
	PSID   psidAdmin       = NULL;
	BOOL   bReturn         = FALSE;

	PRIVILEGE_SET   ps;
	GENERIC_MAPPING GenericMapping;

	PSECURITY_DESCRIPTOR     psdAdmin           = NULL;
	SID_IDENTIFIER_AUTHORITY SystemSidAuthority = SECURITY_NT_AUTHORITY;
   
	__try
	{

		// AccessCheck() requires an impersonation token.
		ImpersonateSelf(SecurityImpersonation);

		if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, FALSE, 
            &hToken)) 
		{

			if (GetLastError() != ERROR_NO_TOKEN)
				__leave;

			// If the thread does not have an access token, we'll 
			// examine the access token associated with the process.
			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, 
				&hToken))
			__leave;
		}

		if (!AllocateAndInitializeSid(&SystemSidAuthority, 2, 
            SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
            0, 0, 0, 0, 0, 0, &psidAdmin))
			 __leave;

		psdAdmin = LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (psdAdmin == NULL)
			__leave;

		if (!InitializeSecurityDescriptor(psdAdmin,
			   SECURITY_DESCRIPTOR_REVISION))
			__leave;
  
		// Compute size needed for the ACL.
		dwACLSize = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) +
           GetLengthSid(psidAdmin) - sizeof(DWORD);

		// Allocate memory for ACL.
		pACL = (PACL)LocalAlloc(LPTR, dwACLSize);
		if (pACL == NULL)
			__leave;

		// Initialize the new ACL.
		if (!InitializeAcl(pACL, dwACLSize, ACL_REVISION2))
			__leave;

		dwAccessMask= ACCESS_READ | ACCESS_WRITE;
      
	     // Add the access-allowed ACE to the DACL.
		 if (!AddAccessAllowedAce(pACL, ACL_REVISION2,
			   dwAccessMask, psidAdmin))
			__leave;

		// Set our DACL to the SD.
		if (!SetSecurityDescriptorDacl(psdAdmin, TRUE, pACL, FALSE))
	        __leave;

		// AccessCheck is sensitive about what is in the SD; set
		// the group and owner.
		SetSecurityDescriptorGroup(psdAdmin, psidAdmin, FALSE);
		SetSecurityDescriptorOwner(psdAdmin, psidAdmin, FALSE);

		if (!IsValidSecurityDescriptor(psdAdmin))
			__leave;

		dwAccessDesired = ACCESS_READ;

		// 
		// Initialize GenericMapping structure even though we
		// won't be using generic rights.
		// 
		GenericMapping.GenericRead    = ACCESS_READ;
		GenericMapping.GenericWrite   = ACCESS_WRITE;
		GenericMapping.GenericExecute = 0;
		GenericMapping.GenericAll     = ACCESS_READ | ACCESS_WRITE;

		if (!AccessCheck(psdAdmin, hToken, dwAccessDesired, 
           &GenericMapping, &ps, &dwStructureSize, &dwStatus, 
           &bReturn))
		{
     
			__leave;
		}

		RevertToSelf();
   
	}
	__finally
	{

		// Cleanup 
		if (pACL) LocalFree(pACL);
		if (psdAdmin) LocalFree(psdAdmin);  
		if (psidAdmin) FreeSid(psidAdmin);
	}

	
	return bReturn;
}

void CSecurity::LogonUser(char* user, char* password, HANDLE& hToken)
{
	CString str(user);
	char UserDomain[128];

	int pos = str.ReverseFind('\\');
	if( pos != -1)
	{
		strcpy(UserDomain,str.Right(str.GetLength()-pos-1));
		strcat(UserDomain,"@");
		strcat(UserDomain,str.Left(pos));
	}
	else
		strcpy(UserDomain,user);


	BOOL bResult = FALSE;

	if(strcmp(user,"")!=0 && strcmp(user,"SYSTEM")!=0)
	{
		bResult = ::LogonUser(
			UserDomain,
			NULL,
			password,
			LOGON32_LOGON_INTERACTIVE,
			LOGON32_PROVIDER_DEFAULT,
			&hToken);

		if(!bResult)
			CExceptionEx::ThrowError(GetLastError());

		bResult = ImpersonateLoggedOnUser( hToken );
		if(!bResult)
			CExceptionEx::ThrowError(GetLastError());
	}
	
}

