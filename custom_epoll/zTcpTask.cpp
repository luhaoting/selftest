class zTcpTask
{
public: 
	
	zTcpTask(const int sock, const struct sockaddr_in* addr):m_sock(sock, addr){}

	virtual bool msgParse(const void* , int) = 0;
	
	void addEpoll(int kdpdf, _uint32_t events, void* ptr)
	{
		m_sock.addEpoll(kdpdf, events, ptr);
	}
	void delEpoll(int kdpdf, _uint32_t events)
	{
		m_sock.delEpoll(kdpdf, evenets);
	}
	
	bool sendCmd(const void* pstrCmd, int CmdLen)
	{
		m_sock.sendCmd(pstrCmd, CmdLen, bufferd);
	}

	bool sendCmdNoPack(const void* pstrCmd, nCmdLen)
	{
		m_sock.sendCmdNoPack(pstrCmd, CmdLen, bufferd);
	}	
	
	//
	virtual bool ListeningRecv(bool needRecv)
	{
		int retcode = 0;
		
		if(needRecv)
		{
			retcode = m_sock.recvToBuf_NoPoll();
		}

		if(-1 == retcode)
		{
			//±¨´í	
		}
		else
		{
			BYTE pstrCmd[zSocket::MAX_DATASIZE];
			int nLen = m_sock.recvToCmd_NoPoll(pstrCmd, sizeof(pstrCmd));
			if(nlen > 0)
			{
				msgParse(pstrCmd, nLen);
			}
		}
	}

	bool ListeningSend()
	{
		return m_sock.sync();
	}
	 
	const char* getIP() const
	{
		return m_sock.getClientIP();
	}

	const unsigned short getPort() const
	{
		return m_sock.getClientPort();
	}

	const unsigned int getAddr() const
	{
		return m_sock.getClientAddr();
	}

private:
	zSocket m_sock;
	bool bufferd; 
}