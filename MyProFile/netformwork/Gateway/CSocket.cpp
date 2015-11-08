/*
 * CSocket.cpp
 *
 *  Created on: 2014��7��16��
 *      Author: luhaoting
 */

#include "CSocket.h"

namespace Net
{

CSocket::CSocket():m_nSocket(0)
{
	// TODO Auto-generated constructor stub

}

CSocket::~CSocket()
{
	// TODO Auto-generated destructor stub

}

bool CSocket::SetNonBlock()
{
#ifdef WIN32
	unsigned long iMode = 1;
	// If iMode = 0, blocking is enabled;
	// If iMode != 0, non-blocking mode is enabled.
	if(ioctlsocket(m_nSocket, FIONBIO, &iMode) < 0)
	{
		Log::NetLog("Socket Set NonBlock Error");
		close(m_nSocket);
		return false;
	}
	return true;
#else
	//_linux
#endif
}

std::string CSocket::GetAddr()
{
	return (inet_ntoa(m_addr.sin_addr));
}

bool CSocket::_Connect(const char* strSvrAddr, int nPort)
{
	struct hostent *hp = 0;
	m_nSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(m_nSocket == -1)
	{
		Log::NetLog("Create Socekt Error");
		return false;
	}

	int addr_len = sizeof(m_addr);
	m_addr.sin_family = AF_INET;

	hp = gethostbyname(strSvrAddr);
	if(NULL == hp)
	{
		Log::NetLog("_Connect's strSvrAddr error");
		return false;
	}

	m_addr.sin_port = htons(nPort);
	m_addr.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
	int ret = connect(m_nSocket, (sockaddr*)&m_addr, addr_len);
	if(ret < 0)
	{
		Log::NetLog("connect error");
		return false;
	}

	//TODO ��δ����������ݷ��ͳ�ȥ���ٹر�socket
	/*
	 * TCP_NODELAY ѡ��

	���ø�ѡ��: public void setTcpNoDelay(boolean on) throws SocketException
	��ȡ��ѡ��: public boolean getTcpNoDelay() throws SocketException
	 Ĭ�������, �������ݲ���Negale �㷨. Negale �㷨��ָ���ͷ����͵����ݲ�����������,

	�����ȷ��ڻ�����, �Ȼ����������ٷ���. ������һ�����ݺ�, ��ȴ����շ����������ݵĻ�Ӧ,

	Ȼ���ٷ�����һ������. Negale �㷨�����ڷ��ͷ���Ҫ���ʹ���������, ���ҽ��շ��ἰʱ����

	��Ӧ�ĳ���, �����㷨ͨ�����ٴ������ݵĴ��������ͨ��Ч��.

		 ������ͷ������ط���С����������, ���ҽ��շ���һ��������������Ӧ����, ��ôNegale

	�㷨��ʹ���ͷ����к���. ����GUI ����, ��������Ϸ����(��������Ҫʵʱ���ٿͻ���������

	��), �����������ͻ��. �ͻ������λ�øĶ�����Ϣ��Ҫʵʱ���͵���������, ����Negale �㷨

	���û���, ��������ʵʱ��Ӧ�ٶ�, ���¿ͻ��������к���.*/
	int nodelay = 1;
	ret = setsockopt(m_nSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&nodelay, sizeof(nodelay));
	if(ret < 0)
	{
		Log::NetLog("setsockopt error");
		return false;
	}
	return true;
}

bool CSocket::_Accept(int nSocekt)
{
#ifdef WIN32
	int nSockAddrLen = sizeof(m_addr);

#else
	socklen_t nSockAddrLen = sizeof(m_addr);
#endif

	m_nSocket = accept(nSocekt, (sockaddr*)&m_addr, &nSockAddrLen);
	if(m_nSocket < 0)
	{
		Log::NetLog("accept, error");
		return false;
	}

	//TODO ��δ����������ݷ��ͳ�ȥ���ٹر�socket
	int nodelay = 1;
	int ret = setsockopt(m_nSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&nodelay, sizeof(nodelay));
	if(ret < 0)
	{
		Log::NetLog("setsockopt error");
		return false;
	}
	return true;
}

void CSocket::wsaStartup()
{
#ifdef WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

} /* namespace Net */
