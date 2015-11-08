/*
 * CEventQueue.h
 *
 *  Created on: 2014年11月13日
 *      Author: luhaoting
 */

#include <iostream>
#include "../Event/CEventBase.h"
#include <list>
#include "../log/CLog.h"

using namespace Net;

//TODO 用于存储事件的队列
namespace Queue
{
	enum Priority
	{
		FIRST,
		SECONED,
		THIRD,
	};

 	typedef struct node
	{
 		struct node * next;
 		struct node * prev;
 		CEventBase * data;
	}Node;

	class CEventQueue final
	{
	public:
		CEventQueue();
		~CEventQueue();

		void push(CEventBase *);
		CEventBase* pop();

		static std::list<CEventQueue>& GetPrioQue(int PrioLevel);
	private:
		inline Node* CreateNewNode(CEventBase *);

		Node *m_head;
		Node *m_last_node;
	};
} /* namespace Queue */
