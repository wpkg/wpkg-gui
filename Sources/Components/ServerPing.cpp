// ServerPing.cpp : implementation file
//

#include "stdafx.h"
#include "ServerPing.h"



// CServerPing

CServerPing::CServerPing()
{
	m_bResult = FALSE;
	m_iErrorCode = -1;
}

CServerPing::~CServerPing()
{
}


// CServerPing member functions

void CServerPing::OnConnect(int nErrorCode)
{
	m_iErrorCode = nErrorCode;

	m_bResult = TRUE;
	CAsyncSocket::OnConnect(nErrorCode);
}

BOOL CServerPing::WaitForConnect(CString ip, DWORD timeout)
{
	m_Ip = ip;
	DWORD dwStartTick = GetTickCount();
	
	BOOL bResult = FALSE;

	while(GetTickCount() - dwStartTick < timeout*1000 )
	{
		if(WaitForConnect())
		{
			bResult = TRUE;
			break;
		}
		Sleep(1000);
	}
	
	return bResult;
}

BOOL CServerPing::WaitForConnect()
{
	
	BOOL OK = Create(0,SOCK_STREAM,FD_CONNECT);
	OK = Connect(m_Ip,139);
	DWORD err = -1;
	if(!OK)
	{
		err = CAsyncSocket::GetLastError();
		if(err!=WSAEWOULDBLOCK)
		{
			Close();
			return FALSE;
		}
	}

	m_bResult = FALSE;
	m_iErrorCode = -1;


	while(TRUE)
	{
		MSG msg;
		while(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			DispatchMessage(&msg);
		}
		if(m_bResult)
			break;
		Sleep(1000);
	}

	Close();
	return (m_iErrorCode == 0);
}

