#ifndef __SHTTPD_H__
#define __SHTTPD_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>   /* for sockaddr_in */
#include <netdb.h>        /* for hostent */ 
#include <pthread.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>        /* we want to catch some of these after all */
#include <unistd.h>       /* protos for read, write, close, etc */
#include <dirent.h>       /* for MAXNAMLEN */
#include <limits.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stddef.h>


/*线程的状态值*/
enum{WORKER_INITED, WORKER_RUNNING,WORKER_DETACHING, WORKER_DETACHED,WORKER_IDEL};

struct conf_opts{
	char CGIRoot[128];		/*CGI跟路径*/
	char DefaultFile[128];		/*默认文件名称*/
	char DocumentRoot[128];	/*根文件路径*/
	char ConfigFile[128];		/*配置文件路径和名称*/
	int ListenPort;			/*侦听端口*/
	int MaxClient;			/*最大客户端数量*/
	int TimeOut;				/*超时时间*/
	int InitClient;				/*初始化线程数量*/
};



/* HTTP协议的方法 */
typedef enum SHTTPD_METHOD_TYPE{
	METHOD_GET, 		/*GET     方法*/
	METHOD_POST, 		/*POST   方法*/
	METHOD_PUT, 		/*PUT     方法*/
	METHOD_DELETE, 	/*DELETE方法*/
	METHOD_HEAD,		/*HEAD   方法*/
	METHOD_CGI,		/**CGI方法*/
	METHOD_NOTSUPPORT
}SHTTPD_METHOD_TYPE;

enum {HDR_DATE, HDR_INT, HDR_STRING};	/* HTTP header types		*/

typedef struct shttpd_method{
	SHTTPD_METHOD_TYPE type;
	int name_index;
	
}shttpd_method;

/*
 * For parsing. This guy represents a substring.
 */
typedef struct vec 
{
	char	*ptr;
	int			len;
	SHTTPD_METHOD_TYPE type;
}vec;

/*
 * This thing is aimed to hold values of any type.
 * Used to store parsed headers' values.
 */
#define big_int_t long

struct http_header {
	int		len;		/* Header name length		*/
	int		type;		/* Header type			*/
	size_t		offset;		/* Value placeholder		*/
	char	*name;		/* Header name			*/
};

/*
 * This structure tells how HTTP headers must be parsed.
 * Used by parse_headers() function.
 */
#define	OFFSET(x)	offsetof(struct headers, x)

/*
 * This thing is aimed to hold values of any type.
 * Used to store parsed headers' values.
 */
union variant {
	char		*v_str;
	int		v_int;
	big_int_t	v_big_int;
	time_t		v_time;
	void		(*v_func)(void);
	void		*v_void;
	struct vec	v_vec;
};


#define	URI_MAX		16384		/* Default max request size	*/
/*
 * This guy holds parsed HTTP headers
 */
struct headers {
	union variant	cl;		/* Content-Length:		*/
	union variant	ct;		/* Content-Type:		*/
	union variant	connection;	/* Connection:			*/
	union variant	ims;		/* If-Modified-Since:		*/
	union variant	user;		/* Remote user name		*/
	union variant	auth;		/* Authorization		*/
	union variant	useragent;	/* User-Agent:			*/
	union variant	referer;	/* Referer:			*/
	union variant	cookie;		/* Cookie:			*/
	union variant	location;	/* Location:			*/
	union variant	range;		/* Range:			*/
	union variant	status;		/* Status:			*/
	union variant	transenc;	/* Transfer-Encoding:		*/
};

struct cgi{
	int iscgi;
	struct vec bin;
	struct vec para;	
};

struct worker_ctl;
struct worker_opts{
	pthread_t th;			/*线程的ID号*/
	int flags;				/*线程状态*/
	pthread_mutex_t mutex;/*线程任务互斥*/

	struct worker_ctl *work;	/*本线程的总控结构*/
};

struct worker_conn ;
/*请求结构*/
struct conn_request{
	struct vec	req;		/*请求向量*/
	char *head;			/*请求头部\0'结尾*/
	char *uri;			/*请求URI,'\0'结尾*/
	char rpath[URI_MAX];	/*请求文件的真实地址\0'结尾*/

	int 	method;			/*请求类型*/

	/*HTTP的版本信息*/
	unsigned long major;	/*主版本*/
	unsigned long minor;	/*副版本*/

	struct headers ch;	/*头部结构*/

	struct worker_conn *conn;	/*连接结构指针*/
	int err;
};

/* 响应结构 */
struct conn_response{
	struct vec	res;		/*响应向量*/
	time_t	birth_time;	/*建立时间*/
	time_t	expire_time;/*超时时间*/

	int		status;		/*响应状态值*/
	int		cl;			/*响应内容长度*/

	int 		fd;			/*请求文件描述符*/
	struct stat fsate;		/*请求文件状态*/

	struct worker_conn *conn;	/*连接结构指针*/	
};
struct worker_conn 
{
#define K 1024
	char		dreq[16*K];	/*请求缓冲区*/
	char		dres[16*K];	/*响应缓冲区*/

	int		cs;			/*客户端套接字文件描述符*/
	int		to;			/*客户端无响应时间超时退出时间*/

	struct conn_response con_res;
	struct conn_request con_req;

	struct worker_ctl *work;	/*本线程的总控结构*/
};

struct worker_ctl{
	struct worker_opts opts;
	struct worker_conn conn;
};
struct mine_type{
	char	*extension;
	int 			type;
	int			ext_len;
	char	*mime_type;
};
void Para_Init(int argc, char *argv[]);

int Request_Parse(struct worker_ctl *wctl);
int Request_Handle(struct worker_ctl* wctl);


int Worker_ScheduleRun();
int Worker_ScheduleStop();
void Method_Do(struct worker_ctl *wctl);
void uri_parse(char *src, int len);

struct mine_type* Mine_Type(char *uri, int len, struct worker_ctl *wctl);



#define DBGPRINT printf


#endif /*__SHTTPD_H__*/

