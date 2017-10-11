#include "stdafx.h"
#include "ProgressMessage.h"

#define BUFSIZE 4096

CProgressMessage::CProgressMessage(void)
{
	m_hChildStdoutRd = m_hChildStdoutWr = NULL;
	m_fileMap = NULL;


	m_sTerminationString = "\n";
	m_sStartString = "STATUS";
}

CProgressMessage::~CProgressMessage(void)
{
	if(m_hChildStdoutRd!=INVALID_HANDLE_VALUE)
		CloseHandle(m_hChildStdoutRd);
}

void CProgressMessage::CreatePipe()
{
	SECURITY_ATTRIBUTES saAttr; 
	

	// Set the bInheritHandle flag so pipe handles are inherited. 

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 

	// Create a pipe for the child process's STDOUT. 

	if (! ::CreatePipe(&m_hChildStdoutRd, &m_hChildStdoutWr, &saAttr, 0)) 
		printf("Stdout pipe creation failed\n"); 

	// Ensure the read handle to the pipe for STDOUT is not inherited.

	SetHandleInformation( m_hChildStdoutRd, HANDLE_FLAG_INHERIT, 0);
}





void CProgressMessage::EventHandler(CObject* pSender, CEventArgs& e)
{
	switch(e.m_EventType)
	{
	case e.typeHandle:
		{
			e.m_hFile = m_hChildStdoutWr;

		}
		break;
	case e.typeMessage:
		{

			DWORD dwRead = 0; 
			CHAR chBuf[BUFSIZE+1]; 
			CByteArray InBuffer;

			int endPos = -1;

			// Close the write end of the pipe before reading from the 
			// read end of the pipe. 

			CloseHandle(m_hChildStdoutWr); 

			// Read output from the child process, and write to parent's STDOUT. 

			for (;;) 
			{ 
				ZeroMemory(chBuf,BUFSIZE);
				if( !ReadFile( m_hChildStdoutRd, chBuf, BUFSIZE, &dwRead, 
					NULL) || dwRead == 0) break; 

				CByteArray buf;
				buf.SetSize(dwRead);
				memcpy(buf.GetData(),chBuf,dwRead);
				InBuffer.Append(buf);

				while((endPos=FindEnd(InBuffer,m_sTerminationString)) != -1)
				{
					char data[4096];
					ZeroMemory(data,4096);
					memcpy(data,InBuffer.GetData(),__min(endPos,4096));
					data[endPos]=0;

					if(m_fileMap)
					{
						CString str;
						str = data;
						if(str.Mid(21,6)==m_sStartString)
						{
							str = data + 31;
							m_fileMap->WriteMessage(str);
							TRACE1("%s\n",str);
						}
					}
					InBuffer.RemoveAt(0,endPos+1);
					Sleep(0);
				}
				Sleep(0);
			} 
		}
		break;
	}
}