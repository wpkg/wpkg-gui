#pragma once



// CServerPing command target

class CServerPing : public CAsyncSocket
{
public:

	CServerPing();
	virtual ~CServerPing();
	virtual void OnConnect(int nErrorCode);
private:
	BOOL WaitForConnect();
	BOOL m_bResult;
	int m_iErrorCode;
	CString m_Ip;
public:
	BOOL WaitForConnect(CString ip, DWORD timeout);
};


