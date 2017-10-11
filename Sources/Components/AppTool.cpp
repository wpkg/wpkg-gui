#include "StdAfx.h"
#include "AppTool.h"

CAppTool::CAppTool(void)
{
}

CAppTool::~CAppTool(void)
{
}

void CAppTool::PumpMessages()
{
	// Must call Create() before using the dialog
	CWnd* pWnd = AfxGetMainWnd();
	HWND hwnd = NULL;
	if(pWnd)
		hwnd = pWnd->GetSafeHwnd();
	

	MSG msg;
	
	// Handle dialog messages
	while(PeekMessage(&msg, hwnd, WM_PAINT, WM_PAINT, PM_REMOVE))
	{
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);  
		
	}
}

