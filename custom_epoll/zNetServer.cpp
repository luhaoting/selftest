#include <ext/numeric>

class zNetServer
{
public:
	virtual void newTcpTask(const int sock, const struct sockaddr_in *addr)  //创建任务
	{
		zTcpTask* tcptask = new zTcpTask(taskpool, sock, addr);
		if(tcptask == NULL) 
		{
			TEMP_FAILURE_RETRY(::close(sock));
		}
		if(!taskpool->addVerify(tcptask))
		{
			delete tcptask;	
		}
	}

protectd:
	zNetServer(const string name)
	{
		instance = this;
		servicename = name;
		tcpserver = NULL;
	}

 	bool init (unsigned short port)
	{
		tcpServer = new zTCPServer(serviceName);
		if(tcpServer = NULL) return false;

		if(!tcpServer->bind(serviceName, prot))
			return false;
		
		return true;
	}

	bool serverCallBack()	///循环
	{
		struct sockaddr_in addr;
		int clientSock = tcpServer->accept(&addr);
		if(retcode >= 0)
		{
			newTcpTask(clientSock, &addr);
		}
		return true;
	}

	void final()
	{
		delete tcpServer;
	}

private:
	static zNetService *instance;
	std::string serviceName;
	zTcpServer* tcpServer;     
	zTcpTaskPool* taskpool;  
}