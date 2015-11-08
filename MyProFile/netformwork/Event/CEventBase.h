/*
 * CEventBase.h
 *
 *  Created on: 2014��11��13��
 *      Author: luhaoting
 */

#ifndef CEVENTBASE_H_
#define CEVENTBASE_H_

#include <iostream>
#include <list>

namespace Net
{

typedef void (*PTRFUN)(void *);

//TODO ������I/O�¼��� m_TypeFlag = EV_READ or EV_write;
//m_cb Ϊ��Ϣ�����MsgMgr����Process������������ void* ����ǿת  MsgInfo
//
//struct MsgInfo
//{
//	ID idActor,
//	string msg,
//}
//
//MsgMgr����ϢMsgBaseͨ��ע��  ����MsgMgr��Map<��Ϣͷ����MsgBase����Process()�����������У�>

//MsgMgr::Process �ڵ���(*m_cb)(MsgInfo info)������;
//MsgBase����Process ��MsgMgr::Process�б����ô�Map�л�ȡ Process[], (*Process[0])(ID idActor,string msg);



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
