// wpkginst.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "wpkginst.h"
#include "wpkginstDlg.h"
#include "..\components\secret.h"
#include "..\components\security.h"
#include "..\components\securefile.h"
#include "..\components\parameters.h"

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
	CSecret s;

	// remove secret data and return imediatelly
	if(CString("/remove").CompareNoCase(m_lpCmdLine)==0)
	{
		s.DeleteSecret();
		return TRUE;
	}

	CParameters param;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("WPKG Installer"));

	param.SetCommandLine(m_lpCmdLine);
	param.Compute();


	//TODO: kasowanie = usuñ
	//s.DeleteSecret();
	/////////////////////////
	
	BOOL bSilent = FALSE;
	CString str = param.GetParameter("silent");
	if(str.CompareNoCase("YES")==0)
	{
		bSilent = TRUE;
	}

	if(!CSecurity::IsAdmin())
	{
		if(!bSilent)
			AfxMessageBox(IDS_IS_NOT_ADMIN);
		return FALSE;
	}


	if(!bSilent)
	{
		if(s.LoadSecret())
		{
			s.m_strScriptPath = param.GetParameter("path");
			s.m_strScriptFile = param.GetParameter("file");
			if(s.m_strScriptFile.IsEmpty())
				s.m_strScriptFile = "wpkg.js";
			s.m_strScriptParameters = param.GetParameter("parameters");
			if(s.m_strScriptParameters.IsEmpty())
				s.m_strScriptParameters = " /synchronize /nonotify /quiet";
			s.m_strScriptConnUser = param.GetParameter("PathUser");
			s.m_strScriptConnPassword = param.GetParameter("PathPassword");
			s.m_strScriptExecUser = param.GetParameter("ExecUser");
			if(s.m_strScriptExecUser.IsEmpty())
				s.m_strScriptExecUser = "SYSTEM";
			s.m_strScriptExecPassword = param.GetParameter("execpassword");
			s.m_strScriptVarName1 = param.GetParameter("ScriptVarName1");
			s.m_strScriptVarValue1 = param.GetParameter("ScriptVarValue1");

		}
	}
	

	CWpkgInstDlg dlg;
	dlg.m_strScriptPath = s.m_strScriptPath;
	dlg.m_strScriptFile = s.m_strScriptFile;
	dlg.m_strScriptParameters = s.m_strScriptParameters;
	dlg.m_strScriptConnUser = s.m_strScriptConnUser;
	dlg.m_strScriptConnPassword = s.m_strScriptConnPassword;
	dlg.m_strScriptExecUser = s.m_strScriptExecUser;
	dlg.m_strScriptExecPassword = s.m_strScriptExecPassword;
	dlg.m_strScriptVarName1 = s.m_strScriptVarName1;
	dlg.m_strScriptVarValue1 = s.m_strScriptVarValue1;

	
	INT_PTR nResponse = IDCANCEL;

	if(!bSilent)
	{
		m_pMainWnd = &dlg;
		nResponse = dlg.DoModal();

		s.m_strScriptPath = dlg.m_strScriptPath;
		s.m_strScriptFile = dlg.m_strScriptFile;
		s.m_strScriptParameters = dlg.m_strScriptParameters;
		s.m_strScriptConnUser = dlg.m_strScriptConnUser;
		s.m_strScriptConnPassword = dlg.m_strScriptConnPassword;
		s.m_strScriptExecUser = dlg.m_strScriptExecUser;
		s.m_strScriptExecPassword = dlg.m_strScriptExecPassword;
		s.m_strScriptVarName1 = dlg.m_strScriptVarName1;
		s.m_strScriptVarValue1 = dlg.m_strScriptVarValue1;


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
