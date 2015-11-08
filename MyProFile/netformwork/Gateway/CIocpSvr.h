/*
 * CIocp.h
 *
 *  Created on: 2014Äê12ÔÂ2ÈÕ
 *      Author: luhaoting
 */

#ifndef GATEWAY_CIOCPSVR_H_
#define GATEWAY_CIOCPSVR_H_

#include <Gateway/CNet.h>
#include "./CMultiPlexerBase.h"

namespace Net
{

class CIocpSvr: public CMultiPlexerBase
{
public:
	CIocpSvr();
	virtual ~CIocpSvr();
	virtual void Init(CNet& net);
	virtual void addEvent(CEventBase & event, void *arg);
	virtual void delEvent(CEventBase & event);
	virtual void dispatch();
};

} /* namespace Net */

#endif /* GATEWAY_CIOCPSVR_H_ */
