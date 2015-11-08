/*
 * CEventQueue.h
 *
 *  Created on: 2014��11��13��
 *      Author: luhaoting
 */

#include <iostream>
#include "../Event/CEventBase.h"
#include <list>
#include "../log/CLog.h"

using namespace Net;

//TODO ���ڴ洢�¼��Ķ���
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
