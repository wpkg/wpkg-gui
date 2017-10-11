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
	BOOL OK = Create(0,SOCK_STREAM,FD_CONNECT);
	OK = Connect(ip,139);
	DWORD err = -1;
	if(!OK)
	{
		err = CAsyncSocket::GetLastError();
		if(err==WSAEWOULDBLOCK)
			err = 0;
	}


	m_bResult = FALSE;
	m_iErrorCode = -1;

	DWORD iteration = 0;

	while(iteration<timeout)
	{
		MSG msg;
		while(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			DispatchMessage(&msg);
		}
		if(m_bResult)
			break;
		iteration++;
		Sleep(1000);
	}

	
	return (m_iErrorCode == 0);
	
}
