typedef std::vector<struct epoll_event> epollfdContainer;
typedef std::list<zTCPTask*> zTCPTaskContainer;
typedef zTCPTaskContainer::iterator zTCPTask_IT;

class zThread
{
private:
	zTCPTaskPool *pool;
	zTCPTaskContainer tasks;	
	zTCPTaskContainer::size_type task_count;
	int kdpfd;
	epollfdContainer epfds;
	static const zTCPTaskContainer::size_type connPerThread = 512;	//每个线程的连接数量


	void _add(zTCPTask* task)
	{
		task->addEpoll(kdpfd, EPOLLIN | EPOLLOUT | EPOLLERR| EPOLLPRI, (void*)task);
		tasks.push_back(task);
		task_count = tasks.size();
		if(task_count > epfds.size())
		{
			epfds.resize(task_count+16);
		}
		task->ListeningRecv(false);
	}
	
	void remove(zTCPTask &it)
	{
		(*it)->delEpoll(kdpfd, EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLPRI);
		task.erase(it);
		task_count = tasks.size();
	}

	zThread(zTCPTaskPool *popl, const std::string &name = std::string("main")) : pool(pool)
	{
		task_count = 0;
		kdpfd = epoll_create(connPerThread);
		assert(-1 != kdpfd);
		epfds.resize(onnPerThread);
	}
	~zThread()
	{
		TEMP_FAILURE_RETRY(::close(kdpfd));
	}

	void  run()
	{
		int retcode = epoll_wait(kdpfd, &epfds[0], task_count, 0);
		if(retcode > 0)
		{
			if(int i = 0; i < retcode; i++)
			{
				zTCPTask *task = (zTCPTask*)epfds[i].data.ptr;
				if(epfds[i].events & (EPOLLERR | EPOLLPRI))
				{
					//套接口异常
				}
				else
				{
					if(epfds[i].events & EPOLLIN)
					{
						if(!task->ListeningRecv(true))
							//读操作错误
					}
					if(epfds[i].evnets & EPOLLOUT)
					{
						if(!task->ListeningSend())
							//写操作错误
					}
				}
				epfds[i].events = 0;
			}		
		}
	}
}
