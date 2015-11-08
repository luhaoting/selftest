/*
 * CSocket.h
 *
 *  Created on: 2014��7��16��
 *      Author: luhaoting
 */

#ifndef CSOCKET_H_
#define CSOCKET_H_

#ifdef WIN32
#include <winsock2.h>

#else
#include <netdb.h>
#include <netinet/in.h>
#endif

#include <string>
#include "../log/CLog.h"

namespace Net
{
	class CSocket
	{
	public:
		CSocket();
		virtual ~CSocket();

		bool SetNonBlock();

		std::string GetAddr();
		bool _Connect(const char* strSvrAddr, int nSvrPort);
		bool _Accept(int nSocket);
		bool _Listen(int nPort);
		void _Close();

		void wsaStartup();
	private:
		int m_nSocket;
		sockaddr_in m_addr;
	};
} /* namespace Net */

#endif /* CSOCKET_H_ */
