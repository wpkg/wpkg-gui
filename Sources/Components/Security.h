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
	static void AddDesktopPermission(HANDLE hToken);
	static void LogonUser(char* user,char* password, HANDLE& hToken);

private:
	static BOOL ObtainSid(
        HANDLE hToken,           // Handle to an process access token.
        PSID   *psid             // ptr to the buffer of the logon sid
        );

	static void RemoveSid(
        PSID *psid               // ptr to the buffer of the logon sid
        );

	static BOOL AddTheAceWindowStation(
        HWINSTA hwinsta,         // handle to a windowstation
        PSID    psid             // logon sid of the process
        );

	static BOOL AddTheAceDesktop(
		HDESK hdesk,             // handle to a desktop
        PSID  psid               // logon sid of the process
        );


};

