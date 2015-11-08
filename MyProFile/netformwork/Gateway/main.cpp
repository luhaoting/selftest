/*
 * main.cpp
 *
 *  Created on: 2014年7月13日
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
//		CLog::instance().log("我写这里测试用debug");
//		CLog::instance().setLogFileName("release");
//		CLog::instance().log("我写这里测试用release");
//	}

	std::list<CEventQueue> PrioQue = Queue::CEventQueue::GetPrioQue(Queue::THIRD);
	return 0;
}

