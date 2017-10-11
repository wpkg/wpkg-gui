// wpkginst.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "wpkginst.h"
#include "wpkginstDlg.h"
#include "..\components\secret.h"
#include "..\components\security.h"
#include "..\components\securefile.h"
#include "..\components\parameters.h"
#include "..\components\XmlSettings.h"
#include "wincon.h"
#include ".\wpkginst.h"


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
	m_iCodeReturned = 0;
}


// The one and only CWpkgInstApp object

CWpkgInstApp theApp;


// CWpkgInstApp initialization

BOOL CWpkgInstApp::InitInstance()
{
	
	CWinApp::InitInstance();

	AfxEnableControlContainer();
	AfxInitRichEdit();
	AfxSocketInit();

	::CoInitialize(NULL);

	CParameters param;
	CSecret s;
	BOOL bSilent = TRUE;


	// remove secret data and return imediatelly
	if(CString("/remove").CompareNoCase(m_lpCmdLine)==0)
	{
		//AfxMessageBox("DELETE_SECRET");
		s.DeleteSecret();
		m_iCodeReturned = 0;
		return FALSE;
		
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


	CWpkgInstDlg dlg;
	CString strFile;
	strFile.Empty();

	// import/export feature

	if(CString("").CompareNoCase(m_lpCmdLine)!=0)
	{
		try
		{
			param.SetCommandLine(m_lpCmdLine);
			param.Compute();
			CString str = param.GetParameter("import");
			if(!str.IsEmpty())
			{
				s.Import(str);
				s.StoreSecret();
				m_iCodeReturned = 0;
				return FALSE;
			}
			str = param.GetParameter("export");
			if(!str.IsEmpty())
			{
				s.LoadSecret();
				s.Export(str);
				m_iCodeReturned = 0;
				return FALSE;
			}
		}
		catch(_com_error e)
		{
			CString str;
			str.Format("Error occured while reading parameter from settings file:\n%s",e.ErrorMessage());
			m_iCodeReturned = 2;
			return FALSE;
		}
		catch(CException* e)
		{
			char buffer[1024];
			e->GetErrorMessage(buffer,1024);
			e->Delete();
			printf(buffer);
			m_iCodeReturned = 2;
			return FALSE;
		}
		catch(...)
		{
			printf("Unknown error occured while reading parameter from settings file");
			m_iCodeReturned = 2;
			return FALSE;
		}



		strFile = param.GetParameter("settingsfile");
	}

	// run from command prompt, get configuration from external file
	if(!strFile.IsEmpty())
	{
		try
		{
			bSilent = TRUE;
			s.Import(strFile);
			bSilent = s.m_bSilent;
		}
		catch(_com_error e)
		{
			CString str;
			str.Format("Error occured while reading parameter from settings file:\n%s",e.ErrorMessage());
			if(!bSilent)
				AfxMessageBox(str);
			m_iCodeReturned = 2;
			return FALSE;
		}
		catch(CException* e)
		{
			if(!bSilent)
				e->ReportError();
			e->Delete();
			m_iCodeReturned = 2;
			return FALSE;
		}
		catch(...)
		{
			if(!bSilent)
				AfxMessageBox("Unknown error occured while reading parameter from settings file");
			m_iCodeReturned = 2;
			return FALSE;
		}

		
	}
	else
	{
		// load internal configuration
		s.LoadSecret();
		//s.m_bSilent = FALSE;
		bSilent = FALSE;
	}

	if(!CSecurity::IsAdmin())
	{
		if(!bSilent)
			AfxMessageBox(IDS_IS_NOT_ADMIN);
		m_iCodeReturned = 2;
		return FALSE;
	}


	INT_PTR nResponse = IDCANCEL;

	if(!bSilent)
	{
		m_pMainWnd = &dlg;
		nResponse = dlg.DoModal();
	}
	else
		nResponse = IDOK;

	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
		s.StoreSecret();
		dlg.SaveLogonDelay();
		dlg.SaveLogonMessages();

	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
		m_iCodeReturned = 2;
		return FALSE;
	}


	// removed at 2007-07-23 ////////////////////////////////////////////////////////////////
	//
	//CRegKey key;
	//char value[1024];
	//DWORD size = 1024;
	//key.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\WPKG.ORG\\Settings",KEY_READ);
	//key.QueryValue(value,"Path",&size);
	//key.Close();

	// CSecureFile::SetEmptyDACL(value);
	// CSecureFile::AddAdminsGroupAllAccess(value);

	// ///////////////////////////////////////////////////////////////////////////////////////

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	m_iCodeReturned = 0;
	return FALSE;
	
}

int CWpkgInstApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	CWinApp::ExitInstance();
	return m_iCodeReturned;
}
