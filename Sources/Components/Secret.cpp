#include "StdAfx.h"
#include ".\secret.h"
#include "Settings.h"

BOOL CSecret::m_bNetUseMachineAccount;
CString CSecret::m_strMessageLogo;
CString CSecret::m_strInterruptPwd;


DWORD CSecret::m_dwShutdownDelay;
BOOL CSecret::m_bRunOnShutdown;
CString CSecret::m_strLogFile;
CString CSecret::m_strScriptFile;
CStringArray CSecret::m_strVarArray;
DWORD CSecret::m_dwPriority;
CString CSecret::m_strScriptParameters;
CString CSecret::m_strScriptConnUser;
CString CSecret::m_strScriptConnPassword;
CString CSecret::m_strScriptExecUser;
CString CSecret::m_strScriptExecPassword;
CString CSecret::m_strPreAction;
CString CSecret::m_strPostAction;
BOOL CSecret::m_bShowGUI;
DWORD CSecret::m_dwLogonDelay;

CString CSecret::m_strMessageTitle;
CString CSecret::m_strMessage1;
CString CSecret::m_strMessage2;

BOOL CSecret::m_bSilent;
BOOL CSecret::m_bStopServiceAfterDone;

int CSecret::m_iServerTestingMethod;
DWORD CSecret::m_dwServerPingTimeout;
DWORD CSecret::m_dwServerPingScriptTimeout;
CString CSecret::m_strServerIP;
CString CSecret::m_ServerPingScriptFile;
BOOL CSecret::m_bLaptopMode;
int CSecret::m_iRepeatCountOnFailure;




CSecret::CSecret(void)
{
	m_bSilent = TRUE;
}

CSecret::~CSecret(void)
{
}


void CSecret::ParsePriority(CString str)
{
	if(str.CompareNoCase("idle")==0)
		m_dwPriority = IDLE_PRIORITY_CLASS;
	if(str.CompareNoCase("below-normal")==0)
		m_dwPriority = BELOW_NORMAL_PRIORITY_CLASS;
	if(str.CompareNoCase("normal")==0)
		m_dwPriority = NORMAL_PRIORITY_CLASS;
	if(str.CompareNoCase("above-normal")==0)
		m_dwPriority = ABOVE_NORMAL_PRIORITY_CLASS;
	if(str.CompareNoCase("high")==0)
		m_dwPriority = HIGH_PRIORITY_CLASS;
	if(str.CompareNoCase("realtime")==0)
		m_dwPriority = REALTIME_PRIORITY_CLASS;
}


CString CSecret::FormatPriority()
{
	CString priority = "normal";

	switch(m_dwPriority)
	{
	case IDLE_PRIORITY_CLASS:
		priority = "idle";
		break;
	case BELOW_NORMAL_PRIORITY_CLASS:
		priority = "below-normal";
		break;
	case NORMAL_PRIORITY_CLASS:
		priority = "normal";
		break;
	case ABOVE_NORMAL_PRIORITY_CLASS:
		priority = "above-normal";
		break;
	case HIGH_PRIORITY_CLASS:
		priority = "high";
		break;
	case REALTIME_PRIORITY_CLASS:
		priority = "realtime";
		break;
	}
	return priority;
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

	// if secret does not exist set defult values
	CString str((Status==0)?(lsaValue->Buffer):L"");
	CXmlSettings st;
	st.CreateInstance();
	st.LoadXml(str);
	ParseXml(st);
		


	LsaFreeMemory(lsaValue);
	return Status;
}




DWORD CSecret::StoreSecret()
{
	LSA_HANDLE PolicyHandle;    // open policy handle
	NTSTATUS Status=0;

	LSA_UNICODE_STRING lsaName;
	LSA_UNICODE_STRING lsaValue;
	
	
	CXmlSettings st;
	st.CreateInstance();
	st.Init();
	FormatXml(st);
	CString str = st.GetXml();
		
	WCHAR* name=L"WPKG_SECRET_DATA";

	InitLsaString(&lsaName,name);
	InitLsaString(&lsaValue,str);

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

	LsaClose(PolicyHandle);
	return Status;
	
}

void CSecret::FormatXml(CXmlSettings& st)
{
	
	st.WriteParameter("file",m_strScriptFile);
	st.WriteParameter("net-use-machine-account",m_bNetUseMachineAccount?"YES":"NO");
	
	st.WriteParameter("path-user",m_strScriptConnUser);
	st.WriteParameter("path-password",st.Crypt(m_strScriptConnPassword));
	st.WriteParameter("exec-user",m_strScriptExecUser);
	st.WriteParameter("exec-password",st.Crypt(m_strScriptExecPassword));
	st.WriteParameter("parameters",m_strScriptParameters);
	st.WriteParameter("silent",m_bSilent?"YES":"NO");
	st.WriteParameter("pre-action",m_strPreAction);
	st.WriteParameter("post-action",m_strPostAction);
	st.WriteParameter("show-GUI",m_bShowGUI?"YES":"NO");

	CString tempStr;
	tempStr.Format("%u",m_dwLogonDelay);
	st.WriteParameter("logon-delay",tempStr);
	st.WriteParameter("logon-message-title",m_strMessageTitle);
	st.WriteParameter("logon-message-logo-picture",m_strMessageLogo);

	// copy file to local app folder with normalized name
	CSettings::SaveLogo(m_strMessageLogo);
	
	st.WriteParameter("logon-message-1",m_strMessage1);
	st.WriteParameter("logon-message-2",m_strMessage2);

	
	for(int i=0; i<m_strVarArray.GetCount(); i+=2)
	{
		st.WriteParameterEx(m_strVarArray.GetAt(i),
			m_strVarArray.GetAt(i+1));
	}
	
	st.WriteParameter("priority",FormatPriority());
	st.WriteParameter("stop-service-after-done",m_bStopServiceAfterDone?"YES":"NO");

	// new 2007.11.30
	st.WriteParameter("laptop-mode",m_bLaptopMode?"YES":"NO");
	st.WriteParameter("server-connecting-method",m_iServerTestingMethod==0?"standard":"custom script");
	st.WriteParameter("server-connecting-ip",m_strServerIP);

	tempStr.Format("%u",m_dwServerPingTimeout);
	st.WriteParameter("server-connecting-timeout",tempStr);
	tempStr.Format("%u",m_dwServerPingScriptTimeout);
	st.WriteParameter("server-connecting-script-timeout",tempStr);
	st.WriteParameter("server-connecting-script-file",m_ServerPingScriptFile);
	st.WriteParameter("log-file",m_strLogFile);

	st.WriteParameter("run-on-shutdown",m_bRunOnShutdown?"YES":"NO");
	tempStr.Format("%u",m_dwShutdownDelay);
	st.WriteParameter("shutdown-delay",tempStr);


	st.WriteParameter("logon-interrupt-password",st.Crypt(m_strInterruptPwd));

	tempStr.Format("%u",m_iRepeatCountOnFailure);
	st.WriteParameter("repeat-count-on-failure",tempStr);
}

void CSecret::Export(CString fileName)
{
	CXmlSettings st;
	st.CreateInstance();
	st.Init();
	FormatXml(st);
	st.Save(fileName);
}
void CSecret::Import(CString fileName)
{
	CXmlSettings st;
	CString str;
	st.CreateInstance();
	st.Load(fileName);
	ParseXml(st);
}

void CSecret::ParseXml(CXmlSettings& st)
{
	// in this place insert default values

	// note:
	// the XML parameters are case sensitive
	CString name,value;
	int count;
	
	value = "wpkg.js";
	st.GetParameter("/configuration/file",value);
	m_strScriptFile = value;

	value = "NO";
	st.GetParameter("/configuration/net-use-machine-account",value);

	if(value.CompareNoCase("YES")==0)
		m_bNetUseMachineAccount = TRUE;
	else
		m_bNetUseMachineAccount = FALSE;

					
	st.GetParameterList("/configuration/script-variable",count);

	m_strVarArray.RemoveAll();
	
	for(int i=0; i<count;i++)
	{
		st.GetParameter(i,name,value);
		m_strVarArray.Add(name);
		m_strVarArray.Add(value);
	}
	
	value = " /synchronize /nonotify /quiet";
	st.GetParameter("/configuration/parameters",value);
	m_strScriptParameters = value;
	
	value.Empty();
	st.GetParameter("/configuration/path-user",value);
	m_strScriptConnUser = value;

	value.Empty();
	st.GetParameter("/configuration/path-password",value);
	m_strScriptConnPassword = st.Decrypt(value);

	value = "SYSTEM";
	st.GetParameter("/configuration/exec-user",value);
	m_strScriptExecUser = value;

	value.Empty();
	st.GetParameter("/configuration/exec-password",value);
	m_strScriptExecPassword = st.Decrypt(value);

	value.Empty();
	st.GetParameter("/configuration/pre-action",value);
	m_strPreAction = value;

	value.Empty();
	st.GetParameter("/configuration/post-action",value);
	m_strPostAction = value;

	value = "NO";
	st.GetParameter("/configuration/show-GUI",value);

	if(value.CompareNoCase("YES")==0)
		m_bShowGUI = TRUE;
	else
		m_bShowGUI = FALSE;

	value.Empty();
	st.GetParameter("/configuration/logon-delay",value);
	m_dwLogonDelay = atol(value);

	
	value = "WPKG Software Deployment";
	st.GetParameter("/configuration/logon-message-title",value);
	m_strMessageTitle = value;

	value.Empty();
	st.GetParameter("/configuration/logon-message-logo-picture",value);
	m_strMessageLogo = value;



	value = "WPKG is installing applications and applying settings...";
	st.GetParameter("/configuration/logon-message-1",value);
	m_strMessage1 = value;

	value = "Please wait, don't restart or power off your computer...";
	st.GetParameter("/configuration/logon-message-2",value);
	m_strMessage2 = value;

	value.Empty();
	st.GetParameter("/configuration/priority",value);
	ParsePriority(value);

	value = "YES";
	st.GetParameter("/configuration/silent",value);
	if(value.CompareNoCase("YES")==0)
		m_bSilent = TRUE;
	else
		m_bSilent = FALSE;

	value = "NO";
	st.GetParameter("/configuration/stop-service-after-done",value);
	if(value.CompareNoCase("YES")==0)
		m_bStopServiceAfterDone = TRUE;
	else
		m_bStopServiceAfterDone = FALSE;

	
	value = "NO";
	st.GetParameter("/configuration/laptop-mode",value);
	if(value.CompareNoCase("YES")==0)
		m_bLaptopMode = TRUE;
	else
		m_bLaptopMode = FALSE;

	value = "standard";
	st.GetParameter("/configuration/server-connecting-method",value);
	if(value.CompareNoCase("standard")==0)
		m_iServerTestingMethod = 0;
	else
		m_iServerTestingMethod = 1;


	value.Empty();
	st.GetParameter("/configuration/server-connecting-ip",value);
	m_strServerIP = value;

	value = "3";
	st.GetParameter("/configuration/server-connecting-timeout",value);
	m_dwServerPingTimeout = atol(value);

	value = "3";
	st.GetParameter("/configuration/server-connecting-script-timeout",value);
	m_dwServerPingScriptTimeout = atol(value);

	value.Empty();
	st.GetParameter("/configuration/server-connecting-script-file",value);
	m_ServerPingScriptFile = value;

	value.Empty();
	st.GetParameter("/configuration/log-file",value);
	m_strLogFile = value;
	
	value = "NO";
	st.GetParameter("/configuration/run-on-shutdown",value);
	if(value.CompareNoCase("YES")==0)
		m_bRunOnShutdown = TRUE;
	else
		m_bRunOnShutdown = FALSE;

	value = "10";
	st.GetParameter("/configuration/shutdown-delay",value);
	m_dwShutdownDelay = atol(value);


	value.Empty();
	st.GetParameter("/configuration/logon-interrupt-password",value);
	m_strInterruptPwd = st.Decrypt(value);

	value = "0";
	st.GetParameter("/configuration/repeat-count-on-failure",value);
	m_iRepeatCountOnFailure = atol(value);
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
    CString string
    )
{
	LsaString->Buffer = string.AllocSysString();
	LsaString->Length = (string.GetLength()+1)*2;
	LsaString->MaximumLength=(string.GetLength()+1)*2;
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