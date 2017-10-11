#pragma once


static SID_IDENTIFIER_AUTHORITY 
		siaNTAuthority = SECURITY_NT_AUTHORITY; 

static SECURITY_DESCRIPTOR m_sd; 
static PSID m_pSIDAliasAdmins; 
static PACL m_pNewDACL;


class CSecurity
{

protected:
public:
	static SECURITY_ATTRIBUTES m_sa; 
	CSecurity(void);
	~CSecurity(void);
	static BOOL AllowAllAccesSa();
	static BOOL AllowAdminAccesSa();
	static BOOL IsAdmin();
	static void LogonUser(char* user,char* password, HANDLE& hToken);
};

