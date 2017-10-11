// wpkginst.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "wpkginst.h"
#include "wpkginstDlg.h"
#include "..\components\secret.h"
#include "..\components\security.h"
#include "..\components\securefile.h"
#include "..\components\parameters.h"
#include "XmlSettings.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4996)

// CWpkgInstApp

BEGIN_MESSAGE_MAP(CWpkgInstApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWpkgInstApp construction

CWpkgInstApp::CWpkgInstApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CWpkgInstApp object

CWpkgInstApp theApp;


// CWpkgInstApp initialization

BOOL CWpkgInstApp::InitInstance()
{
	CWinApp::InitInstance();
	
	CParameters param;
	CSecret s;
	s.m_strScriptFile = "wpkg.js";
	s.m_strScriptParameters = " /synchronize /nonotify /quiet";
	s.m_strScriptExecUser = "SYSTEM";

	// remove secret data and return imediatelly
	if(CString("/remove").CompareNoCase(m_lpCmdLine)==0)
	{
		s.DeleteSecret();
		return TRUE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("WPKG Installer"));

	
	///////////////////////////////////////////////////////////
	//
	// TODO: comment 2 lines below (used for test/debug only)
	// s.DeleteSecret();			// to delete secret box
	// AfxMessageBox(m_lpCmdLine);	// to show command line
	//
	// program optional command line (used for test/debug only):
	// --settingsfile=settings.xml
	//
	///////////////////////////////////////////////////////////
	
	BOOL bSilent = FALSE;

	if(CString("").CompareNoCase(m_lpCmdLine)!=0)
	{
		param.SetCommandLine(m_lpCmdLine);
		param.Compute();
		CString strFile = param.GetParameter("settingsfile");
		if(!strFile.IsEmpty())
		{

			CXmlSettings st;
			CString str;

			try
			{
				// note:
				// the XML parameters are case sensitive
				CString name,value;
				int count;

				::CoInitialize(NULL);

				st.CreateInstance();
				st.Load(strFile);
				
				st.GetParameter("/configuration/file",value);
				s.m_strScriptFile = value;
								
				st.GetParameterList("/configuration/script-variable",count);
				for(int i=0; i<count;i++)
				{
					st.GetParameter(i,name,value);
					s.m_strVarArray.Add(name);
					s.m_strVarArray.Add(value);
				}
		
				st.GetParameter("/configuration/silent",value);
				if(value.CompareNoCase("YES")==0)
				{
					bSilent = TRUE;
				}
			
				st.GetParameter("/configuration/parameters",value);
				s.m_strScriptParameters = value;
				
				st.GetParameter("/configuration/path-user",value);
				s.m_strScriptConnUser = value;
				st.GetParameter("/configuration/path-pasword",value);
				s.m_strScriptConnPassword = value;
				st.GetParameter("/configuration/exec-user",value);
				s.m_strScriptExecUser = value;

				st.GetParameter("/configuration/exec-password",value);
				s.m_strScriptExecPassword = value;
				st.GetParameter("/configuration/pre-action",value);
				s.m_strPreAction = value;
				st.GetParameter("/configuration/post-action",value);
				s.m_strPostAction = value;
			
			}
			catch(_com_error e)
			{
				CString str;
				str.Format("Error occured while reading parameter from settings file:\n%s",e.ErrorMessage());
				AfxMessageBox(str);
			}
		}
	}
	else
		s.LoadSecret();

	if(!CSecurity::IsAdmin())
	{
		if(!bSilent)
			AfxMessageBox(IDS_IS_NOT_ADMIN);
		return FALSE;
	}


	CWpkgInstDlg dlg;
	dlg.m_strScriptFile = s.m_strScriptFile;
	dlg.m_strScriptParameters = s.m_strScriptParameters;
	dlg.m_strScriptConnUser = s.m_strScriptConnUser;
	dlg.m_strScriptConnPassword = s.m_strScriptConnPassword;
	dlg.m_strScriptExecUser = s.m_strScriptExecUser;
	dlg.m_strScriptExecPassword = s.m_strScriptExecPassword;
	dlg.AddScriptVarData(s.m_strVarArray);
	dlg.m_strPreAction = s.m_strPreAction;
	dlg.m_strPostAction = s.m_strPostAction;
	dlg.m_bPreAction = !dlg.m_strPreAction.IsEmpty();
	dlg.m_bPostAction = !dlg.m_strPostAction.IsEmpty();

		
	INT_PTR nResponse = IDCANCEL;

	if(!bSilent)
	{
		m_pMainWnd = &dlg;
		nResponse = dlg.DoModal();

		s.m_strScriptFile = dlg.m_strScriptFile;
		s.m_strScriptParameters = dlg.m_strScriptParameters;
		s.m_strScriptConnUser = dlg.m_strScriptConnUser;
		s.m_strScriptConnPassword = dlg.m_strScriptConnPassword;
		s.m_strScriptExecUser = dlg.m_strScriptExecUser;
		s.m_strScriptExecPassword = dlg.m_strScriptExecPassword;
		s.m_strVarArray.RemoveAll();
		dlg.GetScriptVarData(s.m_strVarArray);
		s.m_strPreAction = dlg.m_strPreAction;
		s.m_strPostAction = dlg.m_strPostAction;

	}
	else
		nResponse = IDOK;

	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK

		s.StoreSecret();
	
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
		return FALSE;
	}

	CRegKey key;
	char value[1024];
	DWORD size = 1024;
	key.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\WPKG.ORG\\Settings",KEY_READ);
	key.QueryValue(value,"Path",&size);
	key.Close();
	
	CSecureFile::SetEmptyDACL(value);
	CSecureFile::AddAdminsGroupAllAccess(value);
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}
