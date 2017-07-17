#include "shttpd.h"

static int workersnum = 0;/*工作线程的数量*/
static struct worker_ctl *wctls = NULL;/*线程选项*/
extern struct conf_opts conf_para;
pthread_mutex_t thread_init = PTHREAD_MUTEX_INITIALIZER;
static int WORKER_ISSTATUS(int status);
static void Worker_Init();
static int Worker_Add(int i);
static void Worker_Delete(int i);
static void Worker_Destory();



static void do_work(struct worker_ctl *wctl)
{
	DBGPRINT("==>do_work\n");
	struct timeval tv;		/*超时时间*/
	fd_set rfds;			/*读文件集*/
	int fd = wctl->conn.cs;/*客户端的套接字描述符*/
	struct vec *req = &wctl->conn.con_req.req;/*请求缓冲区向量*/
	
	int retval = 1;

	for(;retval > 0;)
	{
		/*清读文件集,将客户端连接
			描述符放入读文件集*/
		FD_ZERO(&rfds);	
		FD_SET(fd, &rfds);	

		/*设置超时*/
		tv.tv_sec = 300;//conf_para.TimeOut;
		tv.tv_usec = 0;

		/*超时读数据*/
		retval = select(fd + 1, &rfds, NULL, NULL, &tv);
		switch(retval)
		{
			case -1:/*错误*/
				close(fd);
				break;
			case 0:/*超时*/
				close(fd);
				break;
			default:
				printf("select retval:%d\n",retval);
				if(FD_ISSET(fd, &rfds))/*检测文件*/
				{
					memset(wctl->conn.dreq, 0, sizeof(wctl->conn.dreq));
					/*读取客户端数据*/
					req->len = 
						read(wctl->conn.cs, wctl->conn.dreq, sizeof(wctl->conn.dreq));
					req->ptr = wctl->conn.dreq;
					DBGPRINT("Read %d bytes,'%s'\n",req->len,req->ptr);
					if(req->len > 0)
					{
						/*分析客户端的数据*/
						wctl->conn.con_req.err =	Request_Parse(wctl);
						/*处理并响应客户端请求*/
						Request_Handle(wctl);				
					}
					else
					{
						close(fd);
						retval = -1;
					}
				}
		}
	}
	DBGPRINT("<==do_work\n");
}

/*线程处理函数*/
static void *worker(void *arg)
{
	DBGPRINT("==>worker\n");
	struct worker_ctl *ctl = (struct worker_ctl *)arg;
	struct worker_opts *self_opts = &ctl->opts;

	pthread_mutex_unlock(&thread_init);
	/*初始化线程为空闲，等待任务*/
	self_opts->flags = WORKER_IDEL;

	/*如果主控线程没有让此线程退出，则循环处理任务*/
	for(;self_opts->flags != WORKER_DETACHING;)
	{
		//DBGPRINT("work:%d,status:%d\n",(int)self_opts->th,self_opts->flags );
		/*查看是否有任务分配*/
		int err = pthread_mutex_trylock(&self_opts->mutex);
		if(err)
		{
			//DBGPRINT("NOT LOCK\n");
			sleep(1);
			continue;
		}
		else
		{
			/*有任务，do it*/
			DBGPRINT("Do task\n");
			self_opts->flags = WORKER_RUNNING;
			do_work(ctl);
			close(ctl->conn.cs);
			ctl->conn.cs = -1;
			if(self_opts->flags == WORKER_DETACHING)
				break;
			else
				self_opts->flags = WORKER_IDEL;
		}
	}

	/*主控发送退出命令*/
	/*设置状态为已卸载*/
	self_opts->flags = WORKER_DETACHED;
	workersnum--;/*工作线程-1*/

	DBGPRINT("<==worker\n");
	return NULL;
}


/*线程状态*/
static int WORKER_ISSTATUS(int status)
{
	int i = 0;
	for(i = 0; i<conf_para.MaxClient;i++)
	{
		if(wctls[i].opts.flags == status)
			return i;
	}

	return -1;
}


/*初始化线程*/
static void Worker_Init()
{
	DBGPRINT("==>Worker_Init\n");
	int i = 0;
	/*初始化总控参数*/
	wctls = (struct worker_ctl*)malloc(sizeof(struct worker_ctl)*conf_para.MaxClient);
	memset(wctls, 0, sizeof(*wctls)*conf_para.MaxClient);/*清零*/

	/*初始化一些参数*/
	for(i = 0; i<conf_para.MaxClient;i++)
	{
		/*opts&conn结构与worker_ctl结构形成回指针*/
		wctls[i].opts.work = &wctls[i];
		wctls[i].conn.work = &wctls[i];

		/*opts结构部分的初始化*/
		wctls[i].opts.flags = WORKER_DETACHED;
		//wctls[i].opts.mutex = PTHREAD_MUTEX_INITIALIZER;
		pthread_mutex_init(&wctls[i].opts.mutex,NULL);
		pthread_mutex_lock(&wctls[i].opts.mutex);

		/*conn部分的初始化*/
		/*con_req&con_res与conn结构形成回指*/
		wctls[i].conn.con_req.conn = &wctls[i].conn;
		wctls[i].conn.con_res.conn = &wctls[i].conn;
		wctls[i].conn.cs = -1;/*客户端socket连接为空*/
		/*con_req部分初始化*/
		wctls[i].conn.con_req.req.ptr = wctls[i].conn.dreq;
		wctls[i].conn.con_req.head = wctls[i].conn.dreq;
		wctls[i].conn.con_req.uri = wctls[i].conn.dreq;
		/*con_res部分初始化*/
		wctls[i].conn.con_res.fd = -1; 
		wctls[i].conn.con_res.res.ptr = wctls[i].conn.dres;
	}
	
	for(i = 0; i<conf_para.InitClient;i++)
	{
		/*增加规定个数工作线程*/
		Worker_Add(i);
	}

	DBGPRINT("<==Worker_Init\n");
}


/*增加线程*/
static int Worker_Add(int i)
{
	DBGPRINT("==>Worker_Add\n");
	pthread_t th;
	int err = -1;
	if( wctls[i].opts.flags == WORKER_RUNNING)
		return 1;

	pthread_mutex_lock(&thread_init);
	wctls[i].opts.flags = WORKER_INITED;/*状态为已初始化*/
	err = pthread_create(&th, NULL, worker, (void*)&wctls[i]);/*建立线程*/
	
	pthread_mutex_lock(&thread_init);
	pthread_mutex_unlock(&thread_init);
	/*更新线程选项*/	
	wctls[i].opts.th = th;/*线程ID*/
	workersnum++;/*线程数量增加1*/

	DBGPRINT("<==Worker_Add\n");
	return 0;
}

/*减少线程*/
static void Worker_Delete(int i)
{
	DBGPRINT("==>Worker_Delete\n");
	wctls[i].opts.flags = WORKER_DETACHING;/*线程状态改为正在卸载*/
	DBGPRINT("<==Worker_Delete\n");
}

/*销毁线程*/
static void Worker_Destory()
{
	DBGPRINT("==>Worker_Destory\n");
	int i = 0;
	int clean = 0;
	
	for(i=0;i<conf_para.MaxClient;i++)
	{
		DBGPRINT("thread %d,status %d\n",i,wctls[i].opts.flags );
		if(wctls[i].opts.flags != WORKER_DETACHED)
			Worker_Delete(i);
	}

	while(!clean)
	{
		clean = 1;
		for(i = 0; i<conf_para.MaxClient;i++){
			DBGPRINT("thread %d,status %d\n",i,wctls[i].opts.flags );
			if(wctls[i].opts.flags==WORKER_RUNNING 
				|| wctls[i].opts.flags == WORKER_DETACHING)
				clean = 0;
		}
		if(!clean)
			sleep(1);
	}
	DBGPRINT("<==Worker_Destory\n");
}


#define STATUS_RUNNING 1
#define STATSU_STOP 0
static int SCHEDULESTATUS = STATUS_RUNNING;
/*主调度过程,
*	当有客户端连接到来的时候
*	将客户端连接分配给空闲客户端
*	由客户端处理到来的请求
*/
int Worker_ScheduleRun(int ss)
{
	DBGPRINT("==>Worker_ScheduleRun\n");
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	Worker_Init();
	
	int i = 0;

	for(;SCHEDULESTATUS== STATUS_RUNNING;)
	{
		struct timeval tv;		/*超时时间*/
		fd_set rfds;			/*读文件集*/
		//printf("SCHEDULESTATUS:%d\n",SCHEDULESTATUS);
		int retval = -1;

		/*清读文件集,将客户端连接
			描述符放入读文件集*/
		FD_ZERO(&rfds);	
		FD_SET(ss, &rfds);	

		/*设置超时*/
		tv.tv_sec = 0;
		tv.tv_usec = 500000;

		/*超时读数据*/
		retval = select(ss + 1, &rfds, NULL, NULL, &tv);
		switch(retval)
		{
			case -1:/*错误*/
			case 0:/*超时*/
				continue;
				break;
			default:
				if(FD_ISSET(ss, &rfds))/*检测文件*/
				{
					int sc = accept(ss, (struct sockaddr*)&client, &len);
					printf("client comming\n");
					i = WORKER_ISSTATUS(WORKER_IDEL);
					if(i == -1)
					{
						i =  WORKER_ISSTATUS(WORKER_DETACHED);
						if(i != -1)
							Worker_Add( i);
					}
					if(i != -1)
					{
						wctls[i].conn.cs = sc;
						pthread_mutex_unlock(&wctls[i].opts.mutex);
					}				
				}
		}		
	}
	
	DBGPRINT("<==Worker_ScheduleRun\n");
	return 0;
}

/*停止调度过程*/
int Worker_ScheduleStop()
{
	DBGPRINT("==>Worker_ScheduleStop\n");
	SCHEDULESTATUS = STATSU_STOP;
	int i =0;

	Worker_Destory();
	int allfired = 0;
	for(;!allfired;)
	{
		allfired = 1;
		for(i = 0; i<conf_para.MaxClient;i++)
		{
			int flags = wctls[i].opts.flags;
			if(flags == WORKER_DETACHING || flags == WORKER_IDEL)
				allfired = 0;
		}
	}
	
	pthread_mutex_destroy(&thread_init);
	for(i = 0; i<conf_para.MaxClient;i++)
		pthread_mutex_destroy(&wctls[i].opts.mutex);
	free(wctls);
	
	DBGPRINT("<==Worker_ScheduleStop\n");
	return 0;
}

