/*
 * CSocket.cpp
 *
 *  Created on: 2014年7月16日
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

	//TODO 待未发送完的数据发送出去后再关闭socket
	/*
	 * TCP_NODELAY 选项

	设置该选项: public void setTcpNoDelay(boolean on) throws SocketException
	读取该选项: public boolean getTcpNoDelay() throws SocketException
	 默认情况下, 发送数据采用Negale 算法. Negale 算法是指发送方发送的数据不会立即发出,

	而是先放在缓冲区, 等缓存区满了再发出. 发送完一批数据后, 会等待接收方对这批数据的回应,

	然后再发送下一批数据. Negale 算法适用于发送方需要发送大批量数据, 并且接收方会及时作出

	回应的场合, 这种算法通过减少传输数据的次数来提高通信效率.

		 如果发送方持续地发送小批量的数据, 并且接收方不一定会立即发送响应数据, 那么Negale

	算法会使发送方运行很慢. 对于GUI 程序, 如网络游戏程序(服务器需要实时跟踪客户端鼠标的移

	动), 这个问题尤其突出. 客户端鼠标位置改动的信息需要实时发送到服务器上, 由于Negale 算法

	采用缓冲, 大大减低了实时响应速度, 导致客户程序运行很慢.*/
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

	//TODO 待未发送完的数据发送出去后再关闭socket
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
