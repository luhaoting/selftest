/*
 * CEventQueue.cpp
 *
 *  Created on: 2014年11月17日
 *      Author: luhaoting
 */

#include "CEventQueue.h"

namespace Queue
{

	CEventQueue::CEventQueue()
	:m_head(NULL),m_last_node(NULL)
	{
		// TODO Auto-generated constructor stub

	}

	CEventQueue::~CEventQueue()
	{
		// TODO Auto-generated destructor stub
		for(Node *begin = m_head ; begin != NULL ; )
		{
			begin->data->~CEventBase();
			Node *tmp = begin;
			begin = begin->next;
			delete(tmp);
		}
	}


	Node* CEventQueue::CreateNewNode(CEventBase * event)
	{
		Node *n = new Node();	// 需要内存池
		n->data = event;
		n->next = NULL;
		n->prev = NULL;
		return n;
	}

	void CEventQueue::push(CEventBase* event)
	{
		Node *n = CreateNewNode(event);
		if(NULL == m_head )
		{
			m_head = n;
			m_last_node = n;
		}
		else
		{
			//TODO head != NULL 寻找last
			m_last_node->next = n;
			n->prev = m_last_node;
			m_last_node = n;
		}
		return;
	}

	CEventBase* CEventQueue::pop()
	{
		if(NULL == m_head)
		{
			return NULL;
		}

		Node *tmp = m_head;
		m_head = m_head->next;
		if(NULL != m_head)
		{
			m_head->prev = NULL;
		}
		else
		{
			m_last_node = m_head;
		}
		return tmp->data;
	}

		std::list<CEventQueue>& CEventQueue::GetPrioQue(int PrioLevel = (int)THIRD)
		{
			if( PrioLevel > (int)THIRD ||
				(int)FIRST > PrioLevel)
			{
				Log::DataLog("Create PrioQue error");
				return NULL;
			}
			std::list<CEventQueue> *PrioQue = new std::list<CEventQueue>();
			for(int i = 0 ; i <= PrioLevel ; ++i )
			{
				CEventQueue *q = new CEventQueue();
				PrioQue->push_back(*q);
			}
			return *PrioQue;
		}

} /* namespace Queue */
