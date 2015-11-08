#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>

#include "zSocket.h"

using namespace std;

class epollServer
{
public:
	epollServer(const string &name);
	~epollServer();
	bool bind(const string &name, const unsigned short port);
	int accept(struct sockaddr_in *addr);

private:
	static const int T_MSEC = 2100;			//轮询超时  毫秒
	static const int MAX_WAITQUEUE = 2000;	//最大等待队列

	string name;							//服务器名称
	int sock = -1;							//套接口
	int kdpfd = 0;							//epoll文件句柄
};