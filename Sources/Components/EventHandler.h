#pragma once

class CEventArgs
{
public:
	CEventArgs(void)
	{
		m_hFile = NULL;
	}
	virtual ~CEventArgs(void)
	{
	}
public:
	enum EventType
	{
		typeHandle = 0,
		typeMessage

	} m_EventType;

	HANDLE m_hFile;

};


class CEventHandler
{
public:
	CEventHandler(void)
	{
	}
	virtual ~CEventHandler(void)
	{
	}
public:
	virtual void EventHandler(CObject* pSender, CEventArgs& e)
	{
	}

};


class CEventSender
{
public:
	CEventSender(void)
	{
	}
	virtual ~CEventSender(void)
	{
	}
private:
	CPtrArray m_PtrArray;
public:
	void AddObserver(CEventHandler* pObject);
	void RemoveAll();

	virtual void FireEvent(CObject* pSender, CEventArgs& e);
	
};