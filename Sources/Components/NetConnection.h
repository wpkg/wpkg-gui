#pragma once

class CNetConnection
{
public:
	BOOL m_bSilent;
	CNetConnection(void);
	virtual ~CNetConnection(void);
	DWORD DisconnectFromServer(CString RemotePath);
	void AddConnection(CString RemotePath, CString user,
		CString password);
	void NetErrorHandler( DWORD dwErrorCode );

	void Disconnect(CString RemotePath);
};
