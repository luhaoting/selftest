/*
 * main.cpp
 *
 *  Created on: 2014��7��13��
 *      Author: luhaoting
 */
#include <iostream>
#include "../log/CLog.h"
#include "../Include/CEventQueue.h"
#include <list>

using namespace Log;
using namespace Queue;

int main(int argc,char *argv[])
{
//	for(int i = 0 ; i < 1000000 ; ++ i)
//	{
//		CLog::instance().setLogFileName("debug");
//		CLog::instance().log("��д���������debug");
//		CLog::instance().setLogFileName("release");
//		CLog::instance().log("��д���������release");
//	}

	std::list<CEventQueue> PrioQue = Queue::CEventQueue::GetPrioQue(Queue::THIRD);
	return 0;
}

