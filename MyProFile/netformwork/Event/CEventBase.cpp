/*
 * CEventBase.cpp
 *
 *  Created on: 2014��11��13��
 *      Author: luhaoting
 */

#include "CEventBase.h"

namespace Net
{

CEventBase::CEventBase():m_TypeFlag(0), m_cb(NULL), m_arg(NULL)
{

}

CEventBase::~CEventBase()
{
	// TODO Auto-generated destructor stub
}

} /* namespace Net */
