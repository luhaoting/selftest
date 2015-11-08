/*
 * CNetConcrete.cpp
 *
 *  Created on: 2014Äê11ÔÂ13ÈÕ
 *      Author: luhaoting
 */

#include "CNet.h"

namespace Net {

CNet::CNet() :m_pMultiPlexer(NULL),m_pActiveEventPrioQueList(NULL)
{
	// TODO Auto-generated constructor stub

}

CNet::~CNet()
{
	// TODO Auto-generated destructor stub
}

bool CNet::init(int nEventPrioLevel)
{
	m_pActiveEventPrioQueList = CEventQueue::GetPrioQue(nEventPrioLevel);
	if(NULL == m_pActiveEventPrioQueList)
	{
		Log::NetLog("CNet::init() create m_evPrioQue fail");
		return false;
	}
	return true;
}

bool CNet::setMultiPlexer(CMultiPlexerBase *multi_plexer)
{
	if(!multi_plexer)
	{
		return false;
	}

	m_pMultiPlexer = multi_plexer;
	return true;
}

void CNet::addEvent(CEventBase & event, void *arg)
{
	m_pMultiPlexer->addEvent(event, arg);
}

void CNet::delEvent(CEventBase & event)
{
	m_pMultiPlexer->delEvent(event);
}

void CNet::loop()
{
	m_pMultiPlexer->dispatch();
}

void CNet::Register(CEventBase &event)
{
	m_RegEvent.push(&event);
}

} /* namespace Net */
