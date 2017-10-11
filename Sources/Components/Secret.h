#include "lmaccess.h"
#include "lmerr.h"
#include "lmapibuf.h"
#include "lmserver.h"

#pragma once


#define POLICY_GET_PRIVATE_INFORMATION             0x00000004L
#define POLICY_CREATE_ACCOUNT                      0x00000010L
#define POLICY_CREATE_SECRET                       0x00000020L
#define POLICY_LOOKUP_NAMES                        0x00000800L

//
// LSA RPC Context Handle (Opaque form).  Note that a Context Handle is
// always a pointer type unlike regular handles.
//

typedef PVOID LSA_HANDLE, *PLSA_HANDLE;
#ifdef _NTDEF_
// begin_ntifs
typedef UNICODE_STRING LSA_UNICODE_STRING, *PLSA_UNICODE_STRING;
typedef STRING LSA_STRING, *PLSA_STRING;
typedef OBJECT_ATTRIBUTES LSA_OBJECT_ATTRIBUTES, *PLSA_OBJECT_ATTRIBUTES;
// end_ntifs
#else // _NTDEF_
#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif


typedef struct _LSA_UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING;

typedef struct _LSA_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR Buffer;
} LSA_STRING, *PLSA_STRING;

typedef struct _LSA_OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PLSA_UNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} LSA_OBJECT_ATTRIBUTES, *PLSA_OBJECT_ATTRIBUTES;



#endif // _NTDEF_



////////////////////////////////////////////////////////////////////////////
//                                                                        //
// Local Security Policy - Miscellaneous API function prototypes          //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

NTSTATUS
NTAPI
LsaStorePrivateData(
    IN LSA_HANDLE PolicyHandle,
    IN PLSA_UNICODE_STRING KeyName,
    IN PLSA_UNICODE_STRING PrivateData
    );

NTSTATUS
NTAPI
LsaRetrievePrivateData(
    IN LSA_HANDLE PolicyHandle,
    IN PLSA_UNICODE_STRING KeyName,
    OUT PLSA_UNICODE_STRING * PrivateData
    );

NTSTATUS
NTAPI
LsaFreeMemory(
    IN PVOID Buffer
    );

NTSTATUS
NTAPI
LsaClose(
    IN LSA_HANDLE ObjectHandle
    );

NTSTATUS
NTAPI
LsaOpenPolicy(
    IN PLSA_UNICODE_STRING SystemName OPTIONAL,
    IN PLSA_OBJECT_ATTRIBUTES ObjectAttributes,
    IN ACCESS_MASK DesiredAccess,
    IN OUT PLSA_HANDLE PolicyHandle
    );


#ifdef __cplusplus
}
#endif

//#define SECRET_BUFFER_SIZE 4096


class CSecret
{
public:
	CSecret(void);
	virtual ~CSecret(void);
	DWORD LoadSecret();
	DWORD StoreSecret();
	DWORD DeleteSecret();

public:
	CString m_strScriptPath;
	CString m_strScriptFile;
	CString m_strScriptParameters;
	CString m_strScriptConnUser;
	CString m_strScriptConnPassword;
	CString m_strScriptExecUser;
	CString m_strScriptExecPassword;
	CString m_strScriptVarName1;
	CString m_strScriptVarValue1;

private:
	void Load(void);
	void Store(void);

	char* m_pSecretBuffer;
	DWORD m_dwSecretSize;

	NTSTATUS OpenPolicy(LPWSTR ServerName, DWORD DesiredAccess,
		PLSA_HANDLE PolicyHandle);

	void InitLsaString(PLSA_UNICODE_STRING LsaString, LPWSTR String);
	void InitLsaString(PLSA_UNICODE_STRING LsaString,
		LPWSTR String, DWORD size);



public:
	int Serialize(void);
};
