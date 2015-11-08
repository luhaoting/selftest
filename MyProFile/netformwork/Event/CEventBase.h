/*
 * CEventBase.h
 *
 *  Created on: 2014年11月13日
 *      Author: luhaoting
 */

#ifndef CEVENTBASE_H_
#define CEVENTBASE_H_

#include <iostream>
#include <list>

namespace Net
{

typedef void (*PTRFUN)(void *);

//TODO 举例：I/O事件， m_TypeFlag = EV_READ or EV_write;
//m_cb 为消息处理的MsgMgr：：Process（）处理函数， void* 类型强转  MsgInfo
//
//struct MsgInfo
//{
//	ID idActor,
//	string msg,
//}
//
//MsgMgr：消息MsgBase通过注册  进入MsgMgr的Map<消息头，（MsgBase：：Process()，处理函数队列）>

//MsgMgr::Process 在调用(*m_cb)(MsgInfo info)被调用;
//MsgBase：：Process 在MsgMgr::Process中被调用从Map中获取 Process[], (*Process[0])(ID idActor,string msg);



enum EVENT_TYPE
{
	EV_READ,
	EV_WRITE,
	EV_TIMEOUT,
	EV_SIG,
};

class CEventBase
{
public:
	CEventBase();
	virtual ~CEventBase();
	void Register();
private:
	int m_TypeFlag;
	PTRFUN m_cb;
	void *m_arg;

};

} /* namespace Net */

#endif /* CEVENTBASE_H_ */
