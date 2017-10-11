// SecureFile.h: interface for the CSecureFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECUREFILE_H__C6C6819E_E8AE_11D1_97AE_000000000000__INCLUDED_)
#define AFX_SECUREFILE_H__C6C6819E_E8AE_11D1_97AE_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <aclapi.h>

class CSecureFile  
{
public:
	CSecureFile();
	static DWORD SetEmptyDACL(CString vFileName);
	static DWORD AddAdminsGroupAllAccess(CString FileName );
private:
	static BOOL CSecureFile::SetPrivilege(
	    HANDLE hToken,          // access token handle
		LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
		BOOL bEnablePrivilege   // to enable or disable privilege
		);


	virtual ~CSecureFile();

protected:

	
};

#endif // !defined(AFX_SECUREFILE_H__C6C6819E_E8AE_11D1_97AE_000000000000__INCLUDED_)
