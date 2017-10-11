// wpkglogon.h : main header file for the wpkglogon DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CwpkglogonApp
// See wpkglogon.cpp for the implementation of this class
//

class CwpkglogonApp : public CWinApp
{
public:
	CwpkglogonApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
