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
	static const int T_MSEC = 2100;			//��ѯ��ʱ  ����
	static const int MAX_WAITQUEUE = 2000;	//���ȴ�����

	string name;							//����������
	int sock = -1;							//�׽ӿ�
	int kdpfd = 0;							//epoll�ļ����
};