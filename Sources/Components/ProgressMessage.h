#pragma once
#include "EventHandler.h"
#include "filemap.h"

class CProgressMessage: CEventHandler
{
public:
	
	CProgressMessage(void);
	virtual ~CProgressMessage(void);
	void CreatePipe();
	void SetSharedMemory(CFileMap* pFile)
	{
		m_fileMap = pFile;
	}
protected:

	static int FindEnd(CByteArray& InBuffer,CString& TerminationString)
	{
		int endPos = -1;
		BYTE b1 = TerminationString.GetAt(0);
		BYTE b2 = 0;
		
		for(int i=0; i<InBuffer.GetSize(); i++)
		{
			b2 = InBuffer.GetAt(i);

			if(b1==b2)
			{
				endPos = i;
				break;
			}
		}
		return endPos;
	};

	virtual void EventHandler(CObject* pSender, CEventArgs& e);
private:
	CString m_sTerminationString;
	CString m_sStartString;
	CFileMap* m_fileMap;
	HANDLE m_hChildStdoutRd, m_hChildStdoutWr;
};
