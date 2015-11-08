#include "epollServer.h"

epollServer::epollServer(const std::string &name):name(name)
{
	kdpfd = epoll_create(1);
	assert(-1 == kdpfd);
}

epollServer::~epollServer()
{
	TEMP_FAILURE_RETRY(::close(kdpfd));
	if(-1 != sock)
	{
		::shutdown(sock, SHUT_RD);
		TEMP_FAILURE_RETRY(::close(sock));
		sock = -1;
	}
}

//绑定监听服务到某一个端口
bool epollServer::bind(const std::string &name, const unsigned short port)
{
	struct sockaddr_in addr;
	if(-1 != sock)
	{//服务器可能已经初始化
		return false;
	}
	
	sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(-1 == sock)
	{//穿件套接口失败
		return false;
	}
	
	int reuse = 1;
	if(-1 == ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)))
	{//不能设置套接口为可重用状态
		TEMP_FAILURE_RETRY(::close(sock));
		sock = -1;
		return false;
	}

	//设置套接口发送接收缓冲，并且服务器的必须在accept之前设置
	socklen_t window_size = 128 * 1024;
	if(-1 == ::setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &window_size, sizeof(window_size)))
	{
		TEMP_FAILURE_RETRY(::close(sock));
		return false;
	}
	if(-1 == ::setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &window_size, sizeof(window_size)))
	{
		TEMP_FAILURE_RETRY(::close(sock));
		return false;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	int retcode = ::bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	if (-1 == retcode)
	{//不能绑定套接口
		TEMP_FAILURE_RETRY(::close(sock));
		sock = -1;
		return false;
	}

	retcode = ::listen(sock, MAX_WAITQUEUE);
	if (-1 == retcode)
	{//监听套接口失败
		TEMP_FAILURE_RETRY(::close(sock));
		sock = -1;
		return false;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = NULL;
	assert(0 == epoll_ctl(kdpfd, EPOLL_CTL_ADD, sock, &ev));
	return true;
}


//接受客户端的连接
//param addr 返回的地址
//return 返回的客户端套接口
int epollServer::accept(sockaddr_in *addr)
{
	socklen_t len = sizeof(struct sockaddr_in);
	memset(addr, sizeof(struct sockaddr_in));

	struct epoll_event ev;
	int rc = epoll_wait(kdpfd, &ev, 1, T_MSEC);
	if (1 == rc && (ev.events & EPOLLIN))
	{//准备好接受
		return TEMP_FAILURE_RETRY(::accept(sock, (struct sockaddr *)addr, &len));
	}
	return -1;
}