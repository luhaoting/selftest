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

	//重新调整缓冲区的大小
	inline void wr_reserve(const unsigned int size);

	inline void checkData()
	{
		if(_maxSize < _currPtr)
		{
			//错误输出
		}
	}

	//向缓冲区填入数据
	inline void put(const unsigned char* buf, const unsigned int size)
	{
		wr_reserve(size);	//确认内存足够
		memcpy(&_buffer[_currPtr], buf, size);  
		_currPtr += size;

		if((unsigned int (-1) - _maxSize) <= size)
		{
			//缓冲异常
		}
	}

	//当前可写的地址
	inline unsigned char* wr_buf ()
	{
		return &_buffer[_currPtr];
	}

	//当前有效数据的开始地址
	inline unsigned char* rd_buf()
	{
		return &_buffer[_offPtr];
	}

	//当前缓冲区是否有数据可读
	inline bool rd_ready()
	{
		checkData();
		return  _currPtr > _offPtr;
	}

	//得到缓冲区数据的大小
	inline unsigned int rd_size() const
	{
		checkData();
		return _currPtr - _offPtr;
	}

	//数据读取后 把内存中数据向前移。。
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

	//缓冲区可写入数据的大小
	inline unsigned int wr_size() const
	{
		return _maxSize - _currPtr;
	}

	//缓冲区写入数据 对缓冲区整理
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

//动态内存缓冲区 可以动态扩展缓冲区大小
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
			//缓冲异常
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
	static const int T_RD_MSEC                      = 2100;						//读取超时的毫秒数
	static const int T_WR_MSEC						= 5100;						//发送超时的毫秒数
	static const int PH_LEN							= sizeof(unsigned int);		//数据包包头大小
	static const unsigned int INCOMPLETE_READ		= 0x00000001;				//上次套接口读取数据没有读完的标志
	static const unsigned int INCOMPLETE_WRITE		= 0x00000002;				//上次套接口写数据没有写完的标志
	static const unsigned int PACKET_MASK			= trunkSize -1;				//数据包长度掩码
	static const unsigned int MAX_DATABUFFERSIZE    = trunkSize -1;				//数据包最大长度 包括包头字节
	static const unsigned int MAX_DATASIZE			= MAX_DATABUFFERSIZE - PH_LEN;	//数据包最大长度

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

	//等待套接口准备好读取操作
	//return -1失败 0超时 1成功 已有数据准备读取
	int waitForRead();

	//等待套接口准备好写入操作
	//return -1失败 0超时 1已经可以写入
	int waitForWrite();

	//检查套接口准备好读取操作
	//return -1失败 0超时 1成功 已经可以读取
	int checkIOForRead();

	//检查套接口准备好写入操作
	//return -1失败 0超时 1已经可以写入
	int checkIOForWrite();

	//根据网卡名编号获取制定网卡的IP地址
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

	//设置没完成标志
	inline void set_flag(unsigned int _f) { bitmask |= _f; }
	//判断是否有没完成标志
	inline bool isset_flag(unsigned int _f) const { return bitmask & _f; }
	//清除状态
	inline void clear_flag(unsigned int _f) { bitmask &= ~_f; } 


	int sock;            			//套接口
	struct sockaddr_in client_addr; 	//远程套接口地址
	struct sockaddr_in local_addr; 		//本地套接口地址

	int rd_msec = 0;	//读数据超时
	int wr_msec = 0;	//写数据超时

	t_BufferCmdQueue _rcv_queue;		//接收缓冲指令队列
	unsigned int _rcv_raw_size;			//接收缓冲解密数据大小
	t_BufferCmdQueue _snd_queue;		//发送数据缓冲区
	t_BufferCmdQueue _enc_queue;		//实际发送数据时的队列

	unsigned int _current_cmd;
	unsigned int bitmask = 0;
};