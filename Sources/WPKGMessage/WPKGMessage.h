// WPKGMessage.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CWPKGMessageApp:
// See WPKGMessage.cpp for the implementation of this class
//

extern BOOL bWorking;
extern BOOL bConnected;
extern BOOL IsWorking();
extern BOOL IsWorkingDone();




class CWPKGMessageApp : public CWinApp
{
public:
	CWPKGMessageApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CWPKGMessageApp theApp;