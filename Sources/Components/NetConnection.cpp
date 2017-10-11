#include "StdAfx.h"
#include ".\netconnection.h"
#include "exceptionex.h"

CNetConnection::CNetConnection(void)
{
	m_bSilent = TRUE;
}

CNetConnection::~CNetConnection(void)
{
}

DWORD CNetConnection::DisconnectFromServer(CString RemotePath)
{
	HANDLE hEnum;
	DWORD counter=0xFFFFFFFF;
	BYTE buffer[1024*16];
	DWORD size=1024*16;


	DWORD error = WNetOpenEnum(	
		RESOURCE_CONNECTED,
		RESOURCETYPE_ANY,
		0,
		0,
		&hEnum);

	error = WNetEnumResource(
		hEnum,
		&counter,
		buffer,
		&size);

	WNetCloseEnum(hEnum);

	NETRESOURCE* nrb=(NETRESOURCE*)buffer;
	CString str;
	CString Server;

	// find in path server name
	int pos = RemotePath.Find('\\',2);
	if(pos!=-1)
		Server = RemotePath.Left(pos);



	for( DWORD i=0; i<counter; i++ )
	{
		str.Empty();
		str=nrb[i].lpRemoteName;
		// our server? - yes: disconnect from here
		if( Server.CompareNoCase( str.Left( Server.GetLength() ) ) != 0 )
			continue;
		if( nrb[i].lpLocalName )
		{
			error = WNetCancelConnection2( nrb[i].lpLocalName,0,1);
		}
		error = WNetCancelConnection2( str,0,1);
	
	}

	return error;
}


void CNetConnection::NetErrorHandler( DWORD dwErrorCode ) 
{ 
	DWORD dwLastError; 
	char lpMsgBuf[1024]="Unknown error.";
    CHAR szProvider[256]; 
 
    if (dwErrorCode != ERROR_EXTENDED_ERROR) 
    { 
		FormatMessage( 
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dwErrorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // Default language
			(LPTSTR) &lpMsgBuf,
			1024,
			NULL );

    } 
	else 
    { 
        WNetGetLastError(&dwLastError, // error code
            (LPSTR) lpMsgBuf,		   // buffer for error description 
            sizeof(lpMsgBuf),		   // size of error buffer
            (LPSTR) szProvider,        // buffer for provider name 
            sizeof(szProvider));       // size of name buffer
 
        
        
    }
	
	if( m_bSilent )
	{
	}
	else
		AfxMessageBox(lpMsgBuf);

} 

void CNetConnection::AddConnection(CString RemotePath, CString user,
	CString password)
{
	CString strRemotePath = RemotePath;

	int pos = RemotePath.Find("\\",2);
	if(pos!=-1)
	{
		pos = RemotePath.Find("\\",pos+1);
		if(pos!=-1)
			strRemotePath = RemotePath.Left(pos);
	}
	
	
		

	
	// disconnect - closing files
	DWORD Error = DisconnectFromServer(strRemotePath);
		
	NETRESOURCE nr;
	nr.dwType = RESOURCETYPE_ANY;
	nr.lpLocalName = NULL ;
	nr.lpRemoteName = strRemotePath.GetBuffer();
	nr.lpProvider = NULL;

	
	Error = WNetAddConnection2( &nr,
		password,
		user,
		0);
		

	if( Error != 0 && Error != ERROR_ALREADY_ASSIGNED )
		CExceptionEx::ThrowError("WNetAddConnection2",Error);
	
}

void CNetConnection::Disconnect(CString RemotePath)
{
	WNetCancelConnection2( RemotePath,0,1);
}