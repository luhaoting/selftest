/*
 * CMultiPlexerBase.h
 *
 *  Created on: 2014Äê11ÔÂ13ÈÕ
 *      Author: luhaoting
 */

#ifndef CMULTIPLEXERBASE_H_
#define CMULTIPLEXERBASE_H_

#include "../Event/CEventBase.h"
#include "CNet.h"

class CNet;

namespace Net
{

class CMultiPlexerBase
{
public:
	CMultiPlexerBase();
	virtual ~CMultiPlexerBase();

	virtual void Init(CNet& net) = 0;
	virtual void addEvent(CEventBase & event, void *arg) = 0;
	virtual void delEvent(CEventBase & event) = 0;
	virtual void dispatch() = 0;
private:
	CNet* m_Net;
};

} /* namespace Net */

#endif /* CMULTIPLEXERBASE_H_ */
