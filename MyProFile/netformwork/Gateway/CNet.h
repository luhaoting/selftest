/*
 * CNetConcrete.h
 *
 *  Created on: 2014年11月13日
 *      Author: luhaoting
 */

#ifndef CNETCONCRETE_H_
#define CNETCONCRETE_H_

#include <iostream>
#include "../Event/CEventBase.h"
#include "../Include/CEventQueue.h"
#include "CMultiPlexerBase.h"
#include <list>

//TODO 前置声明
class CMultiPlexerBase;
using namespace Queue;

namespace Net
{

class CNet
{
public:
	CNet();
	virtual ~CNet();

	bool init(int nEventPrioLevel);
	bool setMultiPlexer(CMultiPlexerBase *);

	void addEvent(CEventBase & event, void *arg);
	void delEvent(CEventBase & event);
	void loop();

	void Register(CEventBase &event);
private:
	CMultiPlexerBase *m_pMultiPlexer;
	CEventQueue m_RegEvent;
	std::list<CEventQueue > m_pActiveEventPrioQueList;
	//CEventSingalInfo *m_pSigInfo; 信号消息
	//CTimeStack *m_pTimeStack;	定时栈
	//上述两个类 需要包含CEventBase类
};

} /* namespace Net */

#endif /* CNETCONCRETE_H_ */
