#include "StdAfx.h"
#include ".\security.h"
#include <aclapi.h>
#include "lmaccess.h"
#include "lmapibuf.h"
#include "exceptionex.h"


#define RTN_OK				0
#define RTN_ERROR			13

#define WINSTA_ALL WINSTA_ACCESSCLIPBOARD|WINSTA_ACCESSGLOBALATOMS|\
	WINSTA_CREATEDESKTOP|WINSTA_ENUMDESKTOPS|\
	WINSTA_ENUMERATE|WINSTA_EXITWINDOWS|\
	WINSTA_READATTRIBUTES|WINSTA_READSCREEN|\
	WINSTA_WRITEATTRIBUTES|DELETE|\
	READ_CONTROL|WRITE_DAC|\
	WRITE_OWNER

#define DESKTOP_ALL DESKTOP_CREATEMENU|DESKTOP_CREATEWINDOW|\
	DESKTOP_ENUMERATE|DESKTOP_HOOKCONTROL|\
	DESKTOP_JOURNALPLAYBACK|DESKTOP_JOURNALRECORD|\
	DESKTOP_READOBJECTS|DESKTOP_SWITCHDESKTOP|\
	DESKTOP_WRITEOBJECTS|DELETE|\
	READ_CONTROL|WRITE_DAC|\
	WRITE_OWNER

#define GENERIC_ACCESS GENERIC_READ|GENERIC_WRITE|\
	GENERIC_EXECUTE | GENERIC_ALL



SECURITY_ATTRIBUTES CSecurity::m_sa;


CSecurity::CSecurity(void)
{
	m_pSIDAliasAdmins = NULL; 
}

CSecurity::~CSecurity(void)
{
}


BOOL CSecurity::ObtainSid(HANDLE hToken, PSID *psid)
{
	BOOL                    bSuccess = FALSE; // assume function will
                                              // fail
    DWORD                   dwIndex;
    DWORD                   dwLength = 0;
    TOKEN_INFORMATION_CLASS tic      = TokenGroups;
    PTOKEN_GROUPS           ptg      = NULL;

    __try
    {
        // 
        // determine the size of the buffer
		// 
        if(!GetTokenInformation(
			hToken,
			tic,
			(LPVOID)ptg,
			0,
			&dwLength
			))
        {
			if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
				ptg = (PTOKEN_GROUPS)HeapAlloc(
                    GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					dwLength
					);

                    if(ptg == NULL)
						__leave;
            }
            else
				__leave;
        }

        // 
        // obtain the groups the access token belongs to
        // 
        if(!GetTokenInformation(
            hToken,tic,(LPVOID)ptg,dwLength,&dwLength))
            __leave;

        // 
        // determine which group is the logon sid
        // 
        for(dwIndex = 0; dwIndex < ptg->GroupCount; dwIndex++)
        {
			if((ptg->Groups[dwIndex].Attributes & SE_GROUP_LOGON_ID) ==  SE_GROUP_LOGON_ID)
            {
				// 
                // determine the length of the sid
                // 
                dwLength = GetLengthSid(ptg->Groups[dwIndex].Sid);

                // 
                // allocate a buffer for the logon sid
                // 
                *psid = (PSID)HeapAlloc(
                    GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					dwLength
					);

				if(*psid == NULL)
					__leave;

				// 
				// obtain a copy of the logon sid
				// 
				if(!CopySid(dwLength, *psid, ptg->Groups[dwIndex].Sid))
					__leave;

				// 
				// break out of the loop because the logon sid has been
				// found
				// 
				break;
			}
		}

		// 
		// indicate success
		// 
		bSuccess = TRUE;
    }
    __finally
    {
		// 
		// free the buffer for the token group
		// 
        if(ptg != NULL)
            HeapFree(GetProcessHeap(), 0, (LPVOID)ptg);
    }

	return bSuccess;
}


void CSecurity::RemoveSid(PSID *psid)
{
    HeapFree(GetProcessHeap(), 0, (LPVOID)*psid);
}

BOOL CSecurity::AddTheAceWindowStation(HWINSTA hwinsta, PSID psid)
{

    ACCESS_ALLOWED_ACE   *pace;
    ACL_SIZE_INFORMATION aclSizeInfo;
    BOOL                 bDaclExist;
    BOOL                 bDaclPresent;
    BOOL                 bSuccess  = FALSE; // assume function will
                                            //fail
    DWORD                dwNewAclSize;
    DWORD                dwSidSize = 0;
    DWORD                dwSdSizeNeeded;
    PACL                 pacl;
    PACL                 pNewAcl;
    PSECURITY_DESCRIPTOR psd       = NULL;
    PSECURITY_DESCRIPTOR psdNew    = NULL;
    PVOID                pTempAce;
    SECURITY_INFORMATION si        = DACL_SECURITY_INFORMATION;
    unsigned int         i;

    __try
	{
        // 
        // obtain the dacl for the windowstation
        // 
        if(!GetUserObjectSecurity(
			hwinsta,
			&si,
			psd,
            dwSidSize,
			&dwSdSizeNeeded
            ))
		{
		
            if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                psd = (PSECURITY_DESCRIPTOR)HeapAlloc(
                    GetProcessHeap(),
                    HEAP_ZERO_MEMORY,
                    dwSdSizeNeeded);

                if(psd == NULL)
                    __leave;

                psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(
                    GetProcessHeap(),
                    HEAP_ZERO_MEMORY,
                    dwSdSizeNeeded
                    );
                if(psdNew == NULL)
                    __leave;

                dwSidSize = dwSdSizeNeeded;

                if(!GetUserObjectSecurity(
                    hwinsta,
                    &si,
                    psd,
                    dwSidSize,
                    &dwSdSizeNeeded
                    ))
                    __leave;
			}
			else
				__leave;

		}
        
        // 
        // create a new dacl
		// 
        if(!InitializeSecurityDescriptor(
            psdNew,
            SECURITY_DESCRIPTOR_REVISION
            ))
            __leave;

        // 
		// get dacl from the security descriptor
        // 
        if(!GetSecurityDescriptorDacl(
            psd,
            &bDaclPresent,
            &pacl,
            &bDaclExist
            ))
            __leave;

        // 
        // initialize
        // 
        ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
        aclSizeInfo.AclBytesInUse = sizeof(ACL);

        // 
        // call only if the dacl is not NULL
        // 
        if(pacl != NULL)
        {
            // get the file ACL size info
            if(!GetAclInformation(
                pacl,
                (LPVOID)&aclSizeInfo,
                sizeof(ACL_SIZE_INFORMATION),
                AclSizeInformation
                ))
                __leave;
        }

        // 
        // compute the size of the new acl
        // 
        dwNewAclSize = aclSizeInfo.AclBytesInUse+(2*sizeof(ACCESS_ALLOWED_ACE))+
			(2*GetLengthSid(psid))-(2*sizeof(DWORD));

        // 
        // allocate memory for the new acl
        // 
        pNewAcl = (PACL)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            dwNewAclSize
            );
        if(pNewAcl == NULL)
            __leave;

        // 
        // initialize the new dacl
        // 
        if(!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
            __leave;

        // 
        // if DACL is present, copy it to a new DACL
        // 
        if(bDaclPresent) // only copy if DACL was present
        {
            // copy the ACEs to our new ACL
			if(aclSizeInfo.AceCount)
            {
				for(i=0; i < aclSizeInfo.AceCount; i++)
                {
					// get an ACE
                    if(!GetAce(pacl, i, &pTempAce))
						__leave;

                    // add the ACE to the new ACL
                    if(!AddAce(
						pNewAcl,
                        ACL_REVISION,
                        MAXDWORD,
                        pTempAce,
						((PACE_HEADER)pTempAce)->AceSize))
                            __leave;
				}
			}
        }

        // 
        // add the first ACE to the windowstation
        // 
        pace = (ACCESS_ALLOWED_ACE *)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
			sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psid) -
            sizeof(DWORD));

        if(pace == NULL)
			__leave;

        pace->Header.AceType  = ACCESS_ALLOWED_ACE_TYPE;
        pace->Header.AceFlags = CONTAINER_INHERIT_ACE|
            INHERIT_ONLY_ACE|OBJECT_INHERIT_ACE;

        pace->Header.AceSize = sizeof(ACCESS_ALLOWED_ACE)+
            (WORD)GetLengthSid(psid) - sizeof(DWORD);

        pace->Mask = GENERIC_ACCESS;

        if(!CopySid(GetLengthSid(psid), &pace->SidStart, psid))
            __leave;

        if(!AddAce(
            pNewAcl,
            ACL_REVISION,
			MAXDWORD,
            (LPVOID)pace,
            pace->Header.AceSize))
            __leave;

        // 
        // add the second ACE to the windowstation
        // 
        pace->Header.AceFlags = NO_PROPAGATE_INHERIT_ACE;
        pace->Mask            = WINSTA_ALL;

        if(!AddAce(
            pNewAcl,
            ACL_REVISION,
            MAXDWORD,
            (LPVOID)pace,
            pace->Header.AceSize))
            __leave;

        // 
        // set new dacl for the security descriptor
        // 
        if(!SetSecurityDescriptorDacl(
            psdNew,
            TRUE,
            pNewAcl,
            FALSE
            ))
            __leave;

        // 
		// set the new security descriptor for the windowstation
		// 
		if(!SetUserObjectSecurity(hwinsta, &si, psdNew))
			__leave;

		// 
		// indicate success
		// 
		bSuccess = TRUE;
	}
    __finally
	{
		// 
        // free the allocated buffers
        // 
        if(pace != NULL)
			HeapFree(GetProcessHeap(), 0, (LPVOID)pace);

        if(pNewAcl != NULL)
			HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

        if (psd != NULL)
            HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

        if (psdNew != NULL)
            HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);

    }
    return bSuccess;
}

BOOL CSecurity::AddTheAceDesktop(HDESK hdesk, PSID psid)
{

    ACL_SIZE_INFORMATION aclSizeInfo;
    BOOL                 bDaclExist;
    BOOL                 bDaclPresent;
    BOOL                 bSuccess  = FALSE; // assume function will
                                            // fail
    DWORD                dwNewAclSize;
    DWORD                dwSidSize = 0;
    DWORD                dwSdSizeNeeded;
    PACL                 pacl;
    PACL                 pNewAcl;
    PSECURITY_DESCRIPTOR psd       = NULL;
    PSECURITY_DESCRIPTOR psdNew    = NULL;
    PVOID                pTempAce;
    SECURITY_INFORMATION si        = DACL_SECURITY_INFORMATION;
    unsigned int         i;

    __try
    {
        // 
        // obtain the security descriptor for the desktop object
        // 
        if(!GetUserObjectSecurity(
            hdesk,
            &si,
            psd,
            dwSidSize,
            &dwSdSizeNeeded))
        {
            if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
				psd = (PSECURITY_DESCRIPTOR)HeapAlloc(
                    GetProcessHeap(),
                    HEAP_ZERO_MEMORY,
					dwSdSizeNeeded);

                if(psd == NULL)
                    __leave;

                psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(
                    GetProcessHeap(),
                    HEAP_ZERO_MEMORY,
                    dwSdSizeNeeded);

                if(psdNew == NULL)
                    __leave;

                dwSidSize = dwSdSizeNeeded;

                if(!GetUserObjectSecurity(
                    hdesk,
                    &si,
                    psd,
                    dwSidSize,
                    &dwSdSizeNeeded
                    ))
                    __leave;
            }
            else
                __leave;
        }

        // 
        // create a new security descriptor
        // 
        if(!InitializeSecurityDescriptor(
            psdNew,
            SECURITY_DESCRIPTOR_REVISION
            ))
			__leave;

        // 
        // obtain the dacl from the security descriptor
        // 
        if(!GetSecurityDescriptorDacl(
            psd,
            &bDaclPresent,
            &pacl,
            &bDaclExist
            ))
            __leave;

        // 
        // initialize
        // 
        ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
        aclSizeInfo.AclBytesInUse = sizeof(ACL);

        // 
        // call only if NULL dacl
        // 
        if(pacl != NULL)
        {
            // 
            // determine the size of the ACL info
            // 
            if(!GetAclInformation(
                pacl,
                (LPVOID)&aclSizeInfo,
                sizeof(ACL_SIZE_INFORMATION),
                AclSizeInformation
                ))
                __leave;
        }

        // 
        // compute the size of the new acl
        // 
		dwNewAclSize = aclSizeInfo.AclBytesInUse +
			sizeof(ACCESS_ALLOWED_ACE) +
            GetLengthSid(psid) - sizeof(DWORD);

        // 
        // allocate buffer for the new acl
        // 
        pNewAcl = (PACL)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            dwNewAclSize
            );

        if(pNewAcl == NULL)
            __leave;

        // 
        // initialize the new acl
        // 
        if(!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
            __leave;

        // 
        // if DACL is present, copy it to a new DACL
        // 
        if(bDaclPresent) // only copy if DACL was present
        {
            // copy the ACEs to our new ACL
            if (aclSizeInfo.AceCount)
            {
                for (i=0; i < aclSizeInfo.AceCount; i++)
                {
                    // get an ACE
                    if (!GetAce(pacl, i, &pTempAce))
						__leave;

                    // add the ACE to the new ACL
                    if (!AddAce(
                            pNewAcl,
                            ACL_REVISION,
                            MAXDWORD,
                            pTempAce,
                            ((PACE_HEADER)pTempAce)->AceSize
                            ))
                            __leave;
                }
            }
        }

        // 
        // add ace to the dacl
        // 
        if(!AddAccessAllowedAce(
            pNewAcl,
            ACL_REVISION,
            DESKTOP_ALL,
            psid
            ))
            __leave;

        // 
        // set new dacl to the new security descriptor
        // 
        if(!SetSecurityDescriptorDacl(
                psdNew,
                TRUE,
                pNewAcl,
                FALSE
                ))
            __leave;

        // 
        // set the new security descriptor for the desktop object
        // 
        if(!SetUserObjectSecurity(hdesk, &si, psdNew))
            __leave;

        // 
        // indicate success
        // 
        bSuccess = TRUE;
    }
    __finally
    {
		// 
        // free buffers
        // 
        if(pNewAcl != NULL)
                HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

        if(psd != NULL)
            HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

        if(psdNew != NULL)
            HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);
    }

    return bSuccess;
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

void CSecurity::AddDesktopPermission(HANDLE hToken)
{
    HDESK hdesk;
    HWINSTA hwinsta;
	PSID psid;

	if(hToken!=NULL)
	{

        // 
        // obtain a handle to the interactive windowstation
        // 
		
        hwinsta = OpenWindowStation(
             "winsta0",
             FALSE,
             READ_CONTROL | WRITE_DAC
             );

        if (hwinsta == NULL)
             CExceptionEx::ThrowError(GetLastError());

        HWINSTA hwinstaold = GetProcessWindowStation();

        // 
        // set the windowstation to winsta0 so that you obtain the
        // correct default desktop
        // 
        if (!SetProcessWindowStation(hwinsta))
             CExceptionEx::ThrowError(GetLastError());

        // 
        // obtain a handle to the "default" desktop
        // 
        hdesk = OpenDesktop(
             "default",
             0,
             FALSE,
             READ_CONTROL | WRITE_DAC |
             DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS
             );

        if (hdesk == NULL)
             CExceptionEx::ThrowError(GetLastError());

        // 
        // obtain the logon sid of the user
        // 
        if (!ObtainSid(hToken, &psid))
             CExceptionEx::ThrowError(GetLastError());

        // 
        // add the user to interactive windowstation
        // 
        if (!AddTheAceWindowStation(hwinsta, psid))
             CExceptionEx::ThrowError(GetLastError());

        // 
        // add user to "default" desktop
        // 
        if (!AddTheAceDesktop(hdesk, psid))
             CExceptionEx::ThrowError(GetLastError());

        // 
        // free the buffer for the logon sid
        // 
        RemoveSid(&psid);

        // 
        // close the handles to the interactive windowstation and desktop
        // 
        CloseWindowStation(hwinsta);

        CloseDesktop(hdesk);
	}
}

void CSecurity::LogonUser(char* user, char* password, HANDLE& hToken)
{
	CString str(user);
	char User[128];
	char Domain[128];

	ZeroMemory(User,128);
	ZeroMemory(Domain,128);

	int pos = str.ReverseFind('\\');
	if( pos != -1)
	{
		strcpy(User,str.Right(str.GetLength()-pos-1));
		strcat(Domain,str.Left(pos));
	}
	else
		strcpy(User,user);

	BOOL bResult = FALSE;

	if(strcmp(user,"")!=0 && strcmp(user,"SYSTEM")!=0)
	{
		bResult = ::LogonUser(
			User,
			Domain,
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

