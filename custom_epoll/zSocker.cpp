#include "zSocket.h"

zSocket::zSocket(const int sock, const struct sockaddr_in* addr)
{
	assert(-1 == sock);
	this->sock = sock;
		
	int len = 0;	
	memset(this->client_addr, 0, sizeof(struct sockaddr_in));
	memset(this->local_addr, 0, sizeof(struct sockaddr_in));

	if(NULL == addr)
	{
		len = sizeof(struct sockaddr)
		getpeername(this->sock, (struct sockaddr*)&this->client_addr, &len);   	   //��ȡԶ�̵�ַ
	}
	else
	{
		memcpy(&this->client_addr, addr, sizeof(struct sockaddr_in));
	}
		
	len = sizeof(struct sockaddr_in);
	getsockname(this->sock, (struct sockaddr*)&this->local_addr, &len);  	  //��ȡ���ص�ַ
	
			
	fcntl(this->sock, F_SETFD, fcntl(this->sock, F_GETFD, 0)| FD_CLOEXEC);

	setNonblock();     //���� ������

	rd_msec = T_RD_MSEC;
	wr_msec = T_WR_MSEC;

	_rcv_raw_size = 0;
	_current_cmd = 0;
	set_flag(INCOMPLETE_READ | INCOMPLETE_WRITE);
}
	
zSocket::~zSocket()
{
	::shutdown(sock, SHUT_RDWR);
	TEMP_FAILURE_RETRY(::close(sock));
	SOCK = -1;
}

bool zSocket::setNonblock()
{
	int fd_flags;
	int nodelay = 1;

	//��δ����������ݷ��ͳ�ȥ���ٹر�socket
	::setsockopt(this->sock, IPPROTO_TCP, TCP_NODELAY, (void*)&nodelay, sizeof(nodelay));

	fd_flags = ::fcntl(this->sock, F_GETFL, 0);
		
	#if define O_NONBLOCK
	fd_flags |= O_NONBLOCK;		//���� ��ȡ�������� ����-1
			
	#elif define O_NDELAY
	fd_flags |= O_NDELAY;		//���� ��ȡ�������� ����0
		
	#elif define O_FNDELAY
	fd_flags |= O_FNDELAY;
	#endif
		
	::fcntl(this->sock, F_SETFL, fd_flags);
}

	
void zSocket::addEpoll(int kdpfd, __uint32_t events, void* ptr)
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.ptr = ptr;
	if(-1 == epoll_ctl(kdpfd, EPOLL_CTL_ADD, sock, &ev))
	{
		char buf[100];
		memset(buf, 0 ,sizeof(buf));
		strerror_r(errno, buf, sizeof(buf));
	}
}

void zSocket::delEpoll(int kdpfd, __uint32_t events)
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.ptr = NULL;
	if(-1 == epoll_ctl(kdpfd, EPOLL_CTL_DEL, sock, &ev))
	{
		char buf[100];
		memset(buf, 0, sizeof(buf));
		strerror_r(errno, buf, sizeof(buf));
	}
}

//�ȴ��׽ӿ�׼���ö�ȡ����
//return -1ʧ�� 0��ʱ 1�ɹ� ��������׼����ȡ
int zSocket::waitForRead()
{
	struct pollfd pfd;
	pfd.fd = sock;
	pfd.events = POLLIN | POLLERR | POLLPRI;
	pfd.revents = 0;

	int retcode = TEMP_FAILURE_RETRY(::poll(&pfd, 1, rd_msec));
	if(retcode > 0 && 0 == (pfd.revents & POLLIN))
		retcode = -1;
	return retcode;
}

//�ȴ��׽ӿ�׼����д�����
//return -1ʧ�� 0��ʱ 1�Ѿ�����д��
int zSocket::waitForWrite()
{
	struct pollfd pfd;
	pfd.fd = sock;
	pfd.events = POLLOUT | POLLERR | POLLPRI;
	pfd.revents = 0;

	int retcode = TEMP_FAILURE_RETRY(::poll(&pfd, 1, wr_msec));
	if(retcode > 0 && 0 == (pfd.revents & POLLOUT))
		retcode = -1;
	return retcode;
}

//����׽ӿ�׼���ö�ȡ����
//return -1ʧ�� 0��ʱ 1�ɹ� �Ѿ����Զ�ȡ
int zSocket::checkIOForRead()
{
	struct pollfd pfd;
	pfd.fd = sock;
	pfd.events = POLLIN | POLLERR | POLLPRI;
	pfd.revents = 0;

	int retcode = TEMP_FAILURE_RETRY(::poll(&pfd, 1, 0));
	if(retcode > 0 && 0 == (pfd.revents & POLLIN))
		retcode = -1;
	return retcode;
}

//����׽ӿ�׼����д�����
//return -1ʧ�� 0��ʱ 1�Ѿ�����д��
int zSocket::checkIOForWrite()
{
	struct pollfd pfd;
	pfd.fd = sock;
	pfd.events = POLLOUT | POLLERR | POLLPRI;
	pfd.revents = 0;

	int retcode = TEMP_FAILURE_RETRY(::poll(&pfd, 1, 0));
	if(retcode > 0 && 0 == (pfd.revents & POLLOUT))
		retcode = -1;
	return retcode;
}

const char* zSocket::getIPByIfName(const char* ifName)
{
	int s;
	struct ifreq ifr;
	static const char* none_ip = "0.0.0.0";

	if(NULL == ifName)
		return none_ip;

	memset(ifr.ifr_name, 0, sizeof(ifr.ifr_name));
	strncpy(ifr.ifr_name, ifName, sizeof(ifr.ifr_name) - 1);
	if(-1 == ioctl(s, SIOCGIFADDR, &ifr))
	{
		TEMP_FAILURE_RETRY(::close(s));
		return none_ip;
	}

	TEMP_FAILURE_RETRY(::close(s));
	return inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr);
}

//����û�д��������
//return -1���ʹ��� 0 ���ͳ�ʱ ������ʾ���͵��ֽ�
int zSocket::sendRawData(const void* pBuffer, const int nSize)
{
	int retcode = 0;
	if(isset_flag(INCOMPLETE_WRITE))
	{
		clear_flag(INCOMPLETE_WRITE);
		goto do_select;
	}

	retcode = TEMP_FAILURE_RETRY(::send(sock, pBuffer, nSize, MSG_NOSIGNAL));
	if(retcode == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
	{
do_select:
		retcode = waitForWrite();
		if(1 == retcode)
			retcode = TEMP_FAILURE_RETRY(::send(sock, pBuffer, nSize, MSG_NOSIGNAL));
		else 
			return retcode;
	}
	if(retcode > 0 && retcode < nSize)
		set_flag(INCOMPLETE_WRITE);
	
	return retcode;
}

//����ԭʼ���� ���Գ�ʱ ������ʱ�䷢�� ֪��������ϻ���ʧ��
bool zSocket::sendRawDataIM(const void* pBuffer, const int nSize)
{
	if(NULL == pBuffer || nSize <= 0) return false;
	int offset = 0;
	do
	{
		int retcode = sendRawData(&((char*)pBuffer)[offset], nSize - offset);
		if(-1 == retcode)
		{
			return false;
		}
		offset += retcode;
	}
	while (offset < nSize);
	return offset == nSize;
}

//����ָ�� 
//param buffer �Ƿ���Ҫ����
bool zSocket::sendCmd(const void* pstrCmd, const int nCmdLen, const bool buffer)
{
	if(NULL == pstrCmd || nCmdLen <= 0) return false;

	bool retval = true;
	if(buffer)
	{
		_snd_queue.put((unsigned char*)pstrCmd, nCmdLen);
		_current_cmd = nCmdLen;
	}
	else
	{
		retval = sendRawDataIM(pstrCmd, nCmdLen);
	}
	return retval;
}

//����ָ��	�����������
//param buffer �Ƿ���Ҫ����
bool zSocket::sendCmdNoPack(const void* pstrCmd, const int nCmdLen, const bool buffer)
{
	if(NULL == pstrCmd || nCmdLen <= 0) return false;

	bool retval = true;
	if(buffer)
	{
		_snd_queue.put((unsigned char*)pstrCmd, nCmdLen);
		_current_cmd = nCmdLen;
	}
	else
	{
		retval = sendRawDataIM(pstrCmd, nCmdLen);
	}

	return retval;
}

//����ԭʼ���ݣ���֤���ô˺���֮ǰ�׽ӿ�������д׼��
//return -1���ܴ��� 0��ʱ ���� ʵ�ʽ��յĳ���
int zSocket::sendRawData_NoPoll(const void* pBuffer, const int nSize)
{
	int retcode = TEMP_FAILURE_RETRY(::send(sock, pBuffer, nSize, MSG_NOSIGNAL));
	if (retcode == -1 && (errno == EAGAIN ||errno == EWOULDBLOCK))
		return 0;
	else if(retcode > 0 && retcode < nSize)
		set_flag(INCOMPLETE_WRITE);
	return retcode;
}

//�����ͻ������е����ݷ��ͳ�ȥ ��Ҫ�ȴ��׽ӿڷ�����֮ǰ������
bool zSocket::sync()
{
	if(_snd_queue.rd_ready())
	{
		_enc_queue.put(_snd_queue.rd_buf(), _snd_queue.rd_size());
		_snd_queue.rd_flip(_snd_queue.rd_size());
	}
	if(_enc_queue.rd_ready())
	{
		int retcode = sendRawData_NoPoll(_enc_queue.rd_buf(), _enc_queue.rd_size());
		if(retcode > 0)
		{
			_enc_queue.rd_flip(retcode);
		}
		else if( -1 == retcode)
		{
			return false;
		}
	}
}

//�����ͻ������е����ݷ��ͳ�ȥ   ѭ������ֱ���������
int zSocket::force_sync()
{
	if(_snd_queue.rd_ready())
	{
		_enc_queue.put(_snd_queue.rd_buf(), _snd_queue.rd_size());
		_snd_queue.rd_flip(_snd_queue.rd_size());
	}
	if(_enc_queue.rd_ready())
	{
		sendRawDataIM(_enc_queue.rd_buf(), _enc_queue.rd_size());
		_enc_queue.reset();
	}
}

//���ô˺���֮ǰ��֤�׽ӿ�׼�����˽��� Ҳ����ʹ��poll��ѯ����
//return -1���ܴ��� 0��ʱ ���� ʵ�ʽ��յĳ���
int zSocket::recvToBuf_NoPoll()
{
	_rcv_queue.wr_reserve(MAX_DATABUFFERSIZE);
	int retcode = TEMP_FAILURE_RETRY(::recv(sock, _rcv_queue.wr_buf(), _rcv_queue.wr_size(), MSG_NOSIGNAL));
	if (retcode == -1 && (errno == EAGAIN ||errno == EWOULDBLOCK))
		return 0;
	if(retcode > 0)
	{
		if ((unsigned int) retcode < _rcv_queue.wr_size())
		{
			set_flag(INCOMPLETE_READ);
		}
		_rcv_queue.wr_flip(retcode);
		_rcv_raw_size += retcode;
	}
	if (0 == retcode)
		retcode = -1;

	return retcode;
}

//���ô˺���֮ǰ��֤�׽ӿ�׼�����˽��� Ҳ����ʹ��poll��ѯ����
//return -1���ܴ��� 0��ʱ ���� ʵ�ʽ��յĳ���
int zSocket::recvToBuf_NoPoll(void* pstrBuffer, const int nBufferLen)
{
	int retcode = TEMP_FAILURE_RETRY(::recv(sock, pstrBuffer, nBufferLen, MSG_NOSIGNAL));
	if (retcode == -1 && (errno == EAGAIN ||errno == EWOULDBLOCK))
		return 0;
	else if(retcode == 0)
		return -1;
	else 
		return retcode;
}

///�������ݵ�������
int zSocket::recvToCmd(void* pstrCmd, const int nCmdLen, const bool wait)
{
	recvToCmd_NoPoll(pstrCmd, nCmdLen);

	int retval = recvToBuf();
	if(-1 == retval || (0 == retval && !wait))
		return retval;
	
	recvToCmd_NoPoll(pstrCmd, nCmdLen);
}

///�ӻ������õ�����
int zSocket::recvToCmd_NoPoll(void* pstrCmd, const int nCmdLen)
{
	if (_rcv_raw_size >= packetMinSize())
	{
		unsigned int  nRecordLen = packetSize(_rcv_queue.rd_buf());
		if(_rcv_raw_size >= nRecordLen)
		{
			int retval = packetUnpack(_rcv_queue.rd_buf(), nRecordLen, (unsigned char*) pstrCmd);
			_rcv_queue.rd_flip(nRecordLen);
			_rcv_raw_size -= nRecordLen;
			return retval;
		}
	}
}

//�������ݵ�������
//return -1���ܴ��� 0��ʱ ���� ʵ�ʽ��յĳ���
int zSocket::recvToBuf()
{
	if(isset_flag(INCOMPLETE_READ))
	{
		clear_flag(INCOMPLETE_READ);
		goto do_select;
	}
	_rcv_queue.wr_reserve(MAX_DATABUFFERSIZE);
	int retcode = TEMP_FAILURE_RETRY(::recv(sock, _rcv_queue.wr_buf(), _rcv_queue.wr_size(), MSG_NOSIGNAL));
	if(retcode == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
	{
do_select:
		retcode = waitForWrite();
		if(1 == retcode)
		   retcode = TEMP_FAILURE_RETRY(::recv(sock, _rcv_queue.wr_buf(), _rcv_queue.wr_size(), MSG_NOSIGNAL));
		else 
			return retcode;
	}
	if(retcode > 0)
	{
		if ((unsigned int) retcode < _rcv_queue.wr_size())
		{
			set_flag(INCOMPLETE_READ);
		}
		_rcv_queue.wr_flip(retcode);
		_rcv_raw_size += retcode;
	}
	if (0 == retcode)
		return -1;
	return retcode;
}

unsigned int zSocket::packetUnpack(unsigned char* in, const unsigned int nPakcetLen, unsigned char* out)
{
	unsigned int recvLen = nPakcetLen -PH_LEN;
	memcpy(out, &(in[PH_LEN]), recvLen);
	return recvLen;
}