// SecureFile.cpp: implementation of the CSecureFile class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SecureFile.h"
#include <lm.h>



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


static PSID pSIDAliasAdmins = NULL; 
static SID_IDENTIFIER_AUTHORITY 
	siaNTAuthority = SECURITY_NT_AUTHORITY; 


CSecureFile::CSecureFile()
{

}

CSecureFile::~CSecureFile()
{

}




DWORD CSecureFile::SetEmptyDACL(CString vFileName)
{
	DWORD dwRes;
	PACL pDacl;

	if ("" == vFileName) 
		return ERROR_INVALID_PARAMETER;

	// create an ACL with no ACEs
	pDacl = (PACL)LocalAlloc(LPTR,1024);

	BOOL OK = InitializeAcl(pDacl,1024,ACL_REVISION);

	// attach the emtpy ACL as the object's DACL
	dwRes = SetNamedSecurityInfo(vFileName.GetBuffer(1024), SE_FILE_OBJECT, 
      0x80000004L /*DACL_SECURITY_INFORMATION*/,  
      NULL, NULL, pDacl, NULL);
	vFileName.ReleaseBuffer(1024);

	// free the buffer returned by SetEntriesInAcl

	LocalFree(pDacl);

	return dwRes;
}



DWORD CSecureFile::AddAdminsGroupAllAccess(CString vFileName)
{
	DWORD dwRes;
	PACL pOldDACL=NULL, pNewDACL=NULL;
	PSECURITY_DESCRIPTOR pSD=NULL;
	EXPLICIT_ACCESS ea[2];

	dwRes = GetNamedSecurityInfo(vFileName.GetBuffer(1024), SE_FILE_OBJECT, 
      DACL_SECURITY_INFORMATION,  
      NULL, NULL, &pOldDACL, NULL, &pSD);
	vFileName.ReleaseBuffer(1024);

 
    if (!AllocateAndInitializeSid(&siaNTAuthority, 
            2,                     /* subauthority count  */ 
                                   /* first subauthority */ 
            SECURITY_BUILTIN_DOMAIN_RID, 
                                   /* second subauthority */ 
            DOMAIN_ALIAS_RID_ADMINS, 
			0, 0, 0, 0, 0, 0, 
            &pSIDAliasAdmins)) 
	{ 

        return GetLastError(); 
    } 

	ZeroMemory( ea, sizeof( EXPLICIT_ACCESS ) * 2 );
	ea[0].grfAccessPermissions = GENERIC_ALL;
    ea[0].grfAccessMode = SET_ACCESS;
    ea[0].grfInheritance = CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;//NO_INHERITANCE;
    ea[0].Trustee.pMultipleTrustee = NULL;
	ea[0].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[0].Trustee.ptstrName = 	(char*)pSIDAliasAdmins;

	char* System = "SYSTEM"; 

	
	BuildExplicitAccessWithName( ea + 1, System, GENERIC_ALL, 
        SET_ACCESS, CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE);//NO_INHERITANCE);
	


	dwRes = SetEntriesInAcl( 2, ea, pOldDACL, &pNewDACL);
	if ( ERROR_SUCCESS != dwRes ) 
		goto Cleanup;


	dwRes = SetNamedSecurityInfo(vFileName.GetBuffer(), SE_FILE_OBJECT, 
      DACL_SECURITY_INFORMATION,  
      NULL, NULL, pNewDACL, NULL);


    if (dwRes == ERROR_ACCESS_DENIED)
    {
		// If the preceding call failed because access was denied, 
		// enable the SE_TAKE_OWNERSHIP_NAME privilege, create a SID for 
		// the Administrators group, take ownership of the object, and 
		// disable the privilege. Then try again to set the object's DACL.
		HANDLE hToken = NULL; 


		// Open a handle to the access token for the calling process.
		if (!OpenProcessToken(GetCurrentProcess(), 
							TOKEN_ADJUST_PRIVILEGES, 
							&hToken)) 
		{
			dwRes = GetLastError(); 
			goto Cleanup; 
		} 

		// Enable the SE_TAKE_OWNERSHIP_NAME privilege.
		if (!SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, TRUE)) 
		{
			dwRes = GetLastError(); 
			goto Cleanup; 
		}

		// Set the owner in the object's security descriptor.
		dwRes = SetNamedSecurityInfo(
			vFileName.GetBuffer(),       // name of the object
			SE_FILE_OBJECT,              // type of object
			OWNER_SECURITY_INFORMATION,  // change only the object's owner
			pSIDAliasAdmins,             // SID of Administrator group
			NULL,
			NULL,
			NULL); 

		if (dwRes != ERROR_SUCCESS) 
		{
			dwRes = GetLastError(); 
			goto Cleanup;
		}
	        
		// Disable the SE_TAKE_OWNERSHIP_NAME privilege.
		if (!SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, FALSE)) 
		{
			dwRes = GetLastError(); 
			goto Cleanup;
		}

		// Try again to modify the object's DACL, now that we are the owner.
		dwRes = SetNamedSecurityInfo(
			vFileName.GetBuffer(),       // name of the object
			SE_FILE_OBJECT,              // type of object
			DACL_SECURITY_INFORMATION,   // change only the object's DACL
			NULL, NULL,                  // do not change owner or group
			pNewDACL,                    // DACL specified
			NULL);                       // do not change SACL


	}


	vFileName.ReleaseBuffer(1024);

Cleanup: 
    if(pSD != NULL) 
        LocalFree((HLOCAL) pSD); 
    if(pNewDACL != NULL) 
        LocalFree((HLOCAL) pNewDACL); 

	FreeSid(pSIDAliasAdmins); 


	return dwRes;

}

BOOL CSecureFile::SetPrivilege(
    HANDLE hToken,          // access token handle
    LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
    BOOL bEnablePrivilege   // to enable or disable privilege
    ) 
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if ( !LookupPrivilegeValue( 
			NULL,            // lookup privilege on local system
			lpszPrivilege,   // privilege to lookup 
			&luid ) )        // receives LUID of privilege
	{
		
		return FALSE; 
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if ( !AdjustTokenPrivileges(
		hToken, 
		FALSE, 
		&tp, 
		sizeof(TOKEN_PRIVILEGES), 
		(PTOKEN_PRIVILEGES) NULL, 
		(PDWORD) NULL) )
	{ 
		
		return FALSE; 
	} 

	return TRUE;
}

