#include <errno.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <queue>
#include <ext/poll_allocator.h>
#include <ext/mt_allocator.h>
#include <encdec/encdec.h>
#include <sys/poll.h>
#include <sys/epoll.h>

const unsigned int trunkSize = 64 * 1024;
#define SIOCGIFADDR _IOW('s', 102, struct ifreq)

template <typename _type>
class ByteBuffer
{
public:
	ByteBuffer();
	~ByteBuffer();

	//���µ����������Ĵ�С
	inline void wr_reserve(const unsigned int size);

	inline void checkData()
	{
		if(_maxSize < _currPtr)
		{
			//�������
		}
	}

	//�򻺳�����������
	inline void put(const unsigned char* buf, const unsigned int size)
	{
		wr_reserve(size);	//ȷ���ڴ��㹻
		memcpy(&_buffer[_currPtr], buf, size);  
		_currPtr += size;

		if((unsigned int (-1) - _maxSize) <= size)
		{
			//�����쳣
		}
	}

	//��ǰ��д�ĵ�ַ
	inline unsigned char* wr_buf ()
	{
		return &_buffer[_currPtr];
	}

	//��ǰ��Ч���ݵĿ�ʼ��ַ
	inline unsigned char* rd_buf()
	{
		return &_buffer[_offPtr];
	}

	//��ǰ�������Ƿ������ݿɶ�
	inline bool rd_ready()
	{
		checkData();
		return  _currPtr > _offPtr;
	}

	//�õ����������ݵĴ�С
	inline unsigned int rd_size() const
	{
		checkData();
		return _currPtr - _offPtr;
	}

	//���ݶ�ȡ�� ���ڴ���������ǰ�ơ���
	inline void rd_flip(unsigned int size)
	{
		_offPtr += size;
		if(_currPtr > _offPtr)
		{
			unsigned int tmp = _currPtr - _offPtr;
			if(_offPtr >= tmp)
			{
				memcpy(&_buffer[0], &_buffer[_offPtr], tmp);
				_offPtr = 0;
				_currPtr = tmp;
			}
		}
		else
		{
			_offPtr = 0;
			_currPtr = 0;
		}
	}

	//��������д�����ݵĴ�С
	inline unsigned int wr_size() const
	{
		return _maxSize - _currPtr;
	}

	//������д������ �Ի���������
	inline wr_flip(const unsigned int size)
	{
		_currPtr += size;
	}

	inline void reset()
	{
		_offPtr = 0;
		_currPtr = 0;
	}

	inline unsigned int maxSize() const
	{
		return _maxSize;
	}

	inline unsigned int offPtr() const
	{
		return _offPtr;
	}

	inline unsigned int currPtr() const
	{
		return _currPtr;
	}
	
private:
	unsigned int _maxSize;
	unsigned int _offPtr;
	unsigned int _currPtr;
	_type _buffer;
};

//��̬�ڴ滺���� ���Զ�̬��չ��������С
typedef ByteBuffer< std::vector<unsigned char> > t_BufferCmdQueue;

template<>
inline void t_BufferCmdQueue::wr_reserve(const unsigned int size)
{
	checkData();
	if(wr_size() < size + 8)
	{
		unsigned int trunkCount = (((size + 8) + trunkSize - 1) / trunkSize);
		unsigned int addSize = trunkCount * trunkSize;
		if((unsigned int (-1) - _maxSize) <= addSize)
		{
			//�����쳣
		}
		_maxSize += addSize;
		_buffer.resize(_maxSize);
	}
}

template<>
t_BufferCmdQueue::ByteBuffer():_maxSize(trunkSize), _offPtr(0), _currPtr(0), _buffer(_maxSize){}


class zSocket
{
public:
	static const int T_RD_MSEC                      = 2100;						//��ȡ��ʱ�ĺ�����
	static const int T_WR_MSEC						= 5100;						//���ͳ�ʱ�ĺ�����
	static const int PH_LEN							= sizeof(unsigned int);		//���ݰ���ͷ��С
	static const unsigned int INCOMPLETE_READ		= 0x00000001;				//�ϴ��׽ӿڶ�ȡ����û�ж���ı�־
	static const unsigned int INCOMPLETE_WRITE		= 0x00000002;				//�ϴ��׽ӿ�д����û��д��ı�־
	static const unsigned int PACKET_MASK			= trunkSize -1;				//���ݰ���������
	static const unsigned int MAX_DATABUFFERSIZE    = trunkSize -1;				//���ݰ���󳤶� ������ͷ�ֽ�
	static const unsigned int MAX_DATASIZE			= MAX_DATABUFFERSIZE - PH_LEN;	//���ݰ���󳤶�

	zSocket(const int sock, const struct sockaddr_in* addr);
	
	~zSocket();

	bool setNonblock();

	int  recvToCmd(void* pstrCmd, const int nCmdLen, const bool wait);
	bool sendCmd(const void* pstrCmd, const int nCmdLen, const bool buffer = false);
	bool sendCmdNoPack(const void* pstrCmd, const int nCmdLen, const bool ByteBuffer =false);
	bool sync();
	void force_sync();
	int recvToBuf_NoPoll();
	int recvToBuf_NoPoll(void* pstrBuffer, const int nBufferLen);
	int recvToCmd_NoPoll(void* pstrCmd, const int nCmdLen);	   
	inline int sendRawData(const void* pBuffer, const int nSize); 
	inline bool sendRawDataIM(const void* pBuffer, const int nSize);
	inline int sendRawData_NoPoll(const void* pBuffer, const int nSize);
	inline bool setNonBlock();
	inline int recvToBuf();

	void addEpoll(int kdpfd, __uint32_t events, void* ptr);

	void delEpoll(int kdpfd, __uint32_t events);

	//�ȴ��׽ӿ�׼���ö�ȡ����
	//return -1ʧ�� 0��ʱ 1�ɹ� ��������׼����ȡ
	int waitForRead();

	//�ȴ��׽ӿ�׼����д�����
	//return -1ʧ�� 0��ʱ 1�Ѿ�����д��
	int waitForWrite();

	//����׽ӿ�׼���ö�ȡ����
	//return -1ʧ�� 0��ʱ 1�ɹ� �Ѿ����Զ�ȡ
	int checkIOForRead();

	//����׽ӿ�׼����д�����
	//return -1ʧ�� 0��ʱ 1�Ѿ�����д��
	int checkIOForWrite();

	//������������Ż�ȡ�ƶ�������IP��ַ
	const char* getIPByIfName(const char* ifName)

	const char* getClientIP()const
	{ return inet_ntoa(client_addr.sin_addr);}	
	
	const unsigned int getClientAddr()const
	{ return client_addr.sin_addr.s_addr; }

	const unsigned short getClientPort() const
	{ return ntohs(client_addr.sin_port); }

	const char* getLocalIP()const
	{ return inet_ntoa(local_addr.sin_addr); }

	const unsigned short getLocalPort() const
	{ return ntohs(local_addr.sin_port); }

	inline unsigned int packetMinSize() const { return PH_LEN; }
	inline unsigned int packetSize(const unsigned char* in) const
	{
		return PH_LEN + ((*((unsigned int*)in)) & PACKET_MASK);
	}
	unsigned int packetUnpack(unsigned char* in, const unsigned int nPakcetLen, unsigned char* out);

private:

	//����û��ɱ�־
	inline void set_flag(unsigned int _f) { bitmask |= _f; }
	//�ж��Ƿ���û��ɱ�־
	inline bool isset_flag(unsigned int _f) const { return bitmask & _f; }
	//���״̬
	inline void clear_flag(unsigned int _f) { bitmask &= ~_f; } 


	int sock;            			//�׽ӿ�
	struct sockaddr_in client_addr; 	//Զ���׽ӿڵ�ַ
	struct sockaddr_in local_addr; 		//�����׽ӿڵ�ַ

	int rd_msec = 0;	//�����ݳ�ʱ
	int wr_msec = 0;	//д���ݳ�ʱ

	t_BufferCmdQueue _rcv_queue;		//���ջ���ָ�����
	unsigned int _rcv_raw_size;			//���ջ���������ݴ�С
	t_BufferCmdQueue _snd_queue;		//�������ݻ�����
	t_BufferCmdQueue _enc_queue;		//ʵ�ʷ�������ʱ�Ķ���

	unsigned int _current_cmd;
	unsigned int bitmask = 0;
};