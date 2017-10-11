#pragma once



// CServerPing command target

class CServerPing : public CAsyncSocket
{
public:

	CServerPing();
	virtual ~CServerPing();
	virtual void OnConnect(int nErrorCode);
private:
	BOOL m_bResult;
	int m_iErrorCode;
public:
	BOOL WaitForConnect(CString ip, DWORD timeout);
};


