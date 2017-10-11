// WPKGMessage.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WPKGMessage.h"
#include "WPKGMessageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWPKGMessageApp

BEGIN_MESSAGE_MAP(CWPKGMessageApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWPKGMessageApp construction

CWPKGMessageApp::CWPKGMessageApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CWPKGMessageApp object

CWPKGMessageApp theApp;


// CWPKGMessageApp initialization

BOOL CWPKGMessageApp::InitInstance()
{
	CWinApp::InitInstance();

	CWPKGMessageDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
