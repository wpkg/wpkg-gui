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
      0x80000004L/*DACL_SECURITY_INFORMATION*/,  
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

        return -1; 
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


	dwRes = SetNamedSecurityInfo(vFileName.GetBuffer(1024), SE_FILE_OBJECT, 
      DACL_SECURITY_INFORMATION,  
      NULL, NULL, pNewDACL, NULL);
	vFileName.ReleaseBuffer(1024);

Cleanup: 
    if(pSD != NULL) 
        LocalFree((HLOCAL) pSD); 
    if(pNewDACL != NULL) 
        LocalFree((HLOCAL) pNewDACL); 

	FreeSid(pSIDAliasAdmins); 


	return dwRes;

}

