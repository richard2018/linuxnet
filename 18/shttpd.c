#include "shttpd.h"

struct conf_opts conf_para={
	/*CGIRoot*/		"/usr/local/var/www/cgi-bin/",
	/*DefaultFile*/	"index.html",
	/*DocumentRoot*/"/usr/local/var/www/",
	/*ConfigFile*/	"/etc/sHTTPd.conf",
	/*ListenPort*/	8080,
	/*MaxClient*/	4,	
	/*TimeOut*/		3,
	/*InitClient*/		2
};
struct vec _shttpd_methods[] = {
	{"GET",		3, METHOD_GET},
	{"POST",		4, METHOD_POST},
	{"PUT",		3, METHOD_PUT},
	{"DELETE",	6, METHOD_DELETE},
	{"HEAD",		4, METHOD_HEAD},
	{NULL,		0}
};

/*SIGINT信号截取函数*/
static void sig_int(int num)
{
	Worker_ScheduleStop();

	return;
}

/*SIGPIPE信号截取函数*/
static void sig_pipe(int num)
{
	return;
}
int do_listen()
{
	struct sockaddr_in server;
	int ss = -1;
	int err = -1;
	int reuse = 1;
	int ret = -1;

	/* 初始化服务器地址 */
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr=htonl(INADDR_ANY);
	server.sin_port = htons(conf_para.ListenPort);   
	
	/* 信号截取函数 */
	signal(SIGINT,  sig_int);
	signal(SIGPIPE, sig_pipe);

	/* 生成套接字文件描述符 */
	ss = socket (AF_INET, SOCK_STREAM, 0);
	if (ss == -1)
	{
		printf("socket() error\n");
		ret = -1;
		goto EXITshttpd_listen;
	}

	/* 设置套接字地址和端口复用 */
	err = setsockopt (ss, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	if (err == -1)
	{
		printf("setsockopt SO_REUSEADDR failed\n");
	}

	/* 绑定IP和套接字描述符 */
	err = bind (ss, (struct sockaddr*)  &server, sizeof(server));
	if (err == -1)
	{
		printf("bind() error\n");
		ret = -2;
		goto EXITshttpd_listen;
	}

	/* 设置服务器侦听队列长度 */
	err = listen(ss, conf_para.MaxClient*2);
	if (err)
	{
		printf ("listen() error\n");
		ret = -3;
		goto EXITshttpd_listen;
	}

	ret = ss;
EXITshttpd_listen:
	return ret;
}



int l_main()
{
	int ss = -1;
	ss = do_listen();
	return 0;
}
int main(int argc, char *argv[])
{
	signal(SIGINT, sig_int);

	Para_Init(argc,argv);

	int s = do_listen();

	Worker_ScheduleRun(s);

	return 0;
}
