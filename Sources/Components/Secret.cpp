#include "StdAfx.h"
#include ".\secret.h"


CSecret::CSecret(void)
{
	m_pSecretBuffer = NULL;
}

CSecret::~CSecret(void)
{
}

DWORD CSecret::LoadSecret()
{
	LSA_HANDLE PolicyHandle;    // open policy handle
	NTSTATUS Status=0;
	LSA_UNICODE_STRING lsaName;
	PLSA_UNICODE_STRING lsaValue=NULL;

	WCHAR* name=L"WPKG_SECRET_DATA";



	InitLsaString(&lsaName,name);
	
    Status = OpenPolicy(
        NULL,					// target machine
        POLICY_CREATE_ACCOUNT |
		POLICY_GET_PRIVATE_INFORMATION |
		POLICY_LOOKUP_NAMES,
        &PolicyHandle			// resultant policy handle
        );


	if( Status )
		return Status;
	
	Status = LsaRetrievePrivateData( PolicyHandle, &lsaName, &lsaValue );

	LsaClose(PolicyHandle);

	if( Status == 0 )
	{
		m_pSecretBuffer = (char*)lsaValue->Buffer;
		m_dwSecretSize = lsaValue->Length;
		Load();
	}

	

	LsaFreeMemory(lsaValue);
	m_pSecretBuffer = NULL;

	return Status;
}

DWORD CSecret::StoreSecret()
{
	LSA_HANDLE PolicyHandle;    // open policy handle
	NTSTATUS Status=0;

	LSA_UNICODE_STRING lsaName;
	LSA_UNICODE_STRING lsaValue;
	
	Store();

	WCHAR* temp = (WCHAR*)m_pSecretBuffer;//m_cSecretBuffer;
		
	WCHAR* name=L"WPKG_SECRET_DATA";

	InitLsaString(&lsaName,name);
	InitLsaString(&lsaValue,temp,m_dwSecretSize);

    Status = OpenPolicy(
                NULL,						// target machine
                POLICY_CREATE_ACCOUNT |
				POLICY_CREATE_SECRET |
				POLICY_LOOKUP_NAMES,
                &PolicyHandle				// resultant policy handle
                );

	if( Status )
		return Status;
    Status = LsaStorePrivateData( PolicyHandle, &lsaName, &lsaValue );

	if(m_pSecretBuffer)
	{
		free(m_pSecretBuffer);
		m_pSecretBuffer = NULL;
	}

	LsaClose(PolicyHandle);
	return Status;
	
}

void CSecret::InitLsaString(
    PLSA_UNICODE_STRING LsaString,
    LPWSTR String, DWORD size
    )
{
    LsaString->Buffer = String;
    LsaString->Length = (USHORT)size;
    LsaString->MaximumLength=(USHORT)size;
}


void CSecret::InitLsaString(
    PLSA_UNICODE_STRING LsaString,
    LPWSTR String
    )
{
    DWORD StringLength;

    if(String == NULL) {
        LsaString->Buffer = NULL;
        LsaString->Length = 0;
        LsaString->MaximumLength = 0;
        return;
    }

    StringLength = lstrlenW(String);
    LsaString->Buffer = String;
    LsaString->Length = (USHORT) StringLength * sizeof(WCHAR);
    LsaString->MaximumLength=(USHORT)(StringLength+1) * sizeof(WCHAR);
}

NTSTATUS CSecret::OpenPolicy(
    LPWSTR ServerName,
    DWORD DesiredAccess,
    PLSA_HANDLE PolicyHandle
    )
{
    LSA_OBJECT_ATTRIBUTES ObjectAttributes;
    LSA_UNICODE_STRING ServerString;
    PLSA_UNICODE_STRING Server;

    //
    // Always initialize the object attributes to all zeroes.
    //
    ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

    if (ServerName != NULL) {
        //
        // Make a LSA_UNICODE_STRING out of the LPWSTR passed in
        //
        InitLsaString(&ServerString, ServerName);
        Server = &ServerString;
    } else {
        Server = NULL;
    }

    //
    // Attempt to open the policy.
    //
    return LsaOpenPolicy(
                Server,
                &ObjectAttributes,
                DesiredAccess,
                PolicyHandle
                );
}


void CSecret::Load(void)
{
	CMemFile mf;
	int varCount;
	mf.Attach((BYTE*)m_pSecretBuffer,m_dwSecretSize);

	CArchive ar(&mf,CArchive::load);
	
	ar>>m_strScriptFile;
	ar>>m_strScriptParameters;
	ar>>m_strScriptConnUser;
	ar>>m_strScriptConnPassword;
	ar>>m_strScriptExecUser;
	ar>>m_strScriptExecPassword;

	ar>>varCount;
	for(int i=0; i<varCount;i+=2)
	{
		CString str;
		ar>>str;
		m_strVarArray.Add(str);
		ar>>str;
		m_strVarArray.Add(str);
	}

	ar>>m_strPreAction;
	ar>>m_strPostAction;

	ar.Close();
	mf.Close();
}

void CSecret::Store(void)
{
	CMemFile mf;
	
	CArchive ar(&mf,CArchive::store);

	ar<<m_strScriptFile;
	ar<<m_strScriptParameters;
	ar<<m_strScriptConnUser;
	ar<<m_strScriptConnPassword;
	ar<<m_strScriptExecUser;
	ar<<m_strScriptExecPassword;

	ar<<(int)m_strVarArray.GetCount();
	for(int i=0; i<m_strVarArray.GetCount();i+=2)
	{
		ar<<m_strVarArray.GetAt(i);
		ar<<m_strVarArray.GetAt(i+1);
	}

	ar<<m_strPreAction;
	ar<<m_strPostAction;
	
	ar.Close();
	m_dwSecretSize = (DWORD)mf.GetLength();

	if(m_pSecretBuffer)
	{
		free(m_pSecretBuffer);
		m_pSecretBuffer = NULL;
	}

	m_pSecretBuffer = (char*)mf.Detach();

}


DWORD CSecret::DeleteSecret()
{
	LSA_HANDLE PolicyHandle;    // open policy handle
	NTSTATUS Status = 0;

	LSA_UNICODE_STRING lsaName;	
	
	WCHAR* name=L"WPKG_SECRET_DATA";

	InitLsaString(&lsaName,name);

    Status = OpenPolicy(
                NULL,						// target machine
                POLICY_CREATE_ACCOUNT |
				POLICY_CREATE_SECRET |
				POLICY_LOOKUP_NAMES,
                &PolicyHandle				// resultant policy handle
                );

	if( Status )
		return Status;

    Status = LsaStorePrivateData( PolicyHandle, &lsaName, NULL );

	LsaClose(PolicyHandle);
	return Status;
	
}