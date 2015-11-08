/*
 * CNetConcrete.h
 *
 *  Created on: 2014��11��13��
 *      Author: luhaoting
 */

#ifndef CNETCONCRETE_H_
#define CNETCONCRETE_H_

#include <iostream>
#include "../Event/CEventBase.h"
#include "../Include/CEventQueue.h"
#include "CMultiPlexerBase.h"
#include <list>

//TODO ǰ������
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
	//CEventSingalInfo *m_pSigInfo; �ź���Ϣ
	//CTimeStack *m_pTimeStack;	��ʱջ
	//���������� ��Ҫ����CEventBase��
};

} /* namespace Net */

#endif /* CNETCONCRETE_H_ */
