#include "stdafx.h"
#include "EventHandler.h"



void CEventSender::AddObserver(CEventHandler* pObject)
{
	m_PtrArray.Add(pObject);
}

void CEventSender::RemoveAll()
{
	m_PtrArray.RemoveAll();
}

void CEventSender::FireEvent(CObject* pSender, CEventArgs& e)
{
	for(INT_PTR i = 0; i<m_PtrArray.GetCount(); i++)
	{
		CEventHandler* object = (CEventHandler*)m_PtrArray.GetAt(i);
		object->EventHandler(pSender,e);

	}

}
