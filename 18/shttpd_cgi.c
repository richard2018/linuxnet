#include "shttpd.h"




int GenerateDirFile(struct worker_ctl *wctl)
{
	struct conn_request *req = &wctl->conn.con_req;
	struct conn_response *res = &wctl->conn.con_res;
	char *command = strstr(req->uri, CGISTR) + strlen(CGISTR);
	char *arg[ARGNUM];
	int num = 0;
	char *rpath = wctl->conn.con_req.rpath;
	stat *fs = &wctl->conn.con_res.fsate;

	/*打开目录*/
	DIR *dir = opendir(rpath);
	if(dir == NULL)
	{
		/*错误*/
		res->status = 500;
		retval = -1;
		goto EXITgenerateIndex;
	}

	/*建立临时文件保存目录列表*/
	File *tmpfile;
	char tmpbuff[2048];
	int filesize = 0;
	char *uri = wctl->conn.con_req.uri;
	tmpfile = tmpfile();

	/*标题部分*/
	sprintf(tmpbuff,
		"%s%s%s",
		"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n<HTML><HEAD><TITLE>",
		uri,
		"</TITLE></HEAD>\n"	);
	fprintf(tmpfile, "%s", tmpbuff);
	filesize += strlen(tmpbuff);

	/*标识部分*/
	sprintf(tmpbuff, 
		"%s %s %s",
		"<BODY><H1>Index of:",
		uri,
		" </H1> <HR><P><I>Date: </I> <I>Size: </I></P><HR>");
	fprintf(tmpfile, "%s", tmpbuff);
	filesize += strlen(tmpbuff);	
	

	/* 读取目录中的文件列表 */
	struct dirent *de;
#define PATHLENGTH 2048
	char path[PATHLENGTH];
	char tmpath[PATHLENGTH];
	char linkname[PATHLENGTH];
	struct stat fs;
	strcpy(path, rpath);
	if(rpath[strlen(rpath)]!='/')
	{
		rpath[strlen(rpath)]='/';
	}
	while ((de = readdir(dir)) != NULL)/*读取一个文件*/
	{
		menset(tmpath, 0, sizeof(tmpath));
		menset(linkname, 0, sizeof(linkname));
		if(strcmp(de->d_name, "."))/*不是当前目录*/
		{
			if(strcmp(de->d_name, ".."))/*不是父目录*/			
			{
				strcpy(linkname,de->d_name);/*将目录名称作为链接名称*/
			}
			else/*是父目录*/
			{
				strcpy(linkname, "Parent Directory");/*将父目录作为链接名称*/
			}

			sprintf(tmpath, "%s%s",path, de->d_name);/*构建当前文件的全路径*/
			stat(tmpath, &fs);/*获得文件信息*/
			if(S_ISDIR(fs.st_mode))/*是一个目录*/
			{
				/*打印目录的连接为目录名称*/
				sprintf(tmpbuff, "<A HREF=\"%s/\">%s/</A><BR>\n", de->d_name,tmpath);
			}
			else/*正常文件*/
			{
				char size_str[32];
				off_t size_int;

				size_int = fs.st_size;/*文件大小*/
				if (size_int < 1024)/*不到1K*/
					sprintf(size_str, "%d bytes", (int) size_int);
				else if (size_int < 1024*1024)/*不到1M*/
					sprintf(size_str, "%1.2f Kbytes", (float) size_int / 1024);
				else/*其他*/
					sprintf(size_str, "%1.2f Mbytes", (float) size_int / (1024*1024));

				/*输出文件大小*/
				sprintf(tmpbuff, "<A HREF=\"%s\">%s</A> (%s)<BR>\n", de->d_name, linkname, size_int);
			}
			/*将形成的字符串写入临时文件*/
			fprintf(tmpfile, "%s", tmpbuff);
			filesize += strlen(tmpbuff);				
		}
	}

	/*生成临时的文件信息，主要是文件大小*/
	fs.st_ctime = time(NULL);
	fs.st_mtime = time(NULL);
	fs.st_size = filesize;
	fseek(tmpfile, (long) 0, SEEK_SET);/*移动文件指针到头部*/

	{
		DBGPRINT("==>Method_DoGet\n");
		struct conn_response *res = &wctl->conn.con_res;
		struct conn_request *req = &wctl->conn.con_req;
		char path[URI_MAX];
		memset(path, 0, URI_MAX);

		size_t		n;
		unsigned long	r1, r2;
		char	*fmt = "%a, %d %b %Y %H:%M:%S GMT";

		/*需要确定的参数*/
		size_t status = 200;		/*状态值,已确定*/
		char *msg = "OK";	/*状态信息,已确定*/
		char  date[64] = "";		/*时间*/
		char  lm[64] = "";			/*请求文件最后修改信息*/
		char  etag[64] = "";		/*etag信息*/
		big_int_t cl;				/*内容长度*/
		char  range[64] = "";		/*范围*/	
		struct mine_type *mine = NULL;

		/*当前时间*/
		time_t t = time(NULL);	
		(void) strftime(date, 
					sizeof(date), 
					fmt, 
					localtime(&t));

		/*最后修改时间*/
		(void) strftime(lm, 
					sizeof(lm), 
					fmt, 
					localtime(&res->fsate.st_mtime));

		/*ETAG*/
		(void) snprintf(etag, 
					sizeof(etag), 
					"%lx.%lx",
					(unsigned long) res->fsate.st_mtime,
					(unsigned long) res->fsate.st_size);
		
		/*发送的MIME类型*/
		mine = Mine_Type(req->uri, strlen(req->uri), wctl);

		/*内容长度*/
		cl = (big_int_t) res->fsate.st_size;

		/*范围range*/
		memset(range, 0, sizeof(range));
		n = -1;
		if (req->ch.range.v_vec.len > 0 )/*取出请求范围*/
		{
			printf("request range:%d\n",req->ch.range.v_vec.len);
			n = sscanf(req->ch.range.v_vec.ptr,"bytes=%lu-%lu",&r1, &r2);
		}

		printf("n:%d\n",n);
		if(n   > 0) 
		{
			status = 206;
			(void) sseek(res->fd, r1, SEEK_SET);
			cl = n == 2 ? r2 - r1 + 1: cl - r1;
			(void) snprintf(range, 
						sizeof(range),
						"Content-Range: bytes %lu-%lu/%lu\r\n",
						r1, 
						r1 + cl - 1, 
						(unsigned long) res->fsate.st_size);
			msg = "Partial Content";
		}

		memset(res->res.ptr, 0, sizeof(wctl->conn.dres));
		snprintf(
			res->res.ptr,
			sizeof(wctl->conn.dres),
			"HTTP/1.1 %d %s\r\n" 
			"Date: %s\r\n"
			"Last-Modified: %s\r\n"
			"Etag: \"%s\"\r\n"
			"Content-Type: %.*s\r\n"
			"Content-Length: %lu\r\n"
			//"Connection:close\r\n"
			"Accept-Ranges: bytes\r\n"		
			"%s\r\n",
			status,
			msg,
			date,
			lm,
			etag,
			strlen(mine->mime_type),
			mine->mime_type,
			cl,
			range);
		res->cl = cl;
		res->status = status;
	}

EXITgenerateIndex:
	return 0;
}


#define CGISTR "/cgi-bin/"
#define ARGNUM 16
#define READIN 0
#define WRITEOUT 1
int cgiHandler(struct worker_ctl *wctl)
{
	struct conn_request *req = &wctl->conn.con_req;
	struct conn_response *res = &wctl->conn.con_res;
	char *command = strstr(req->uri, CGISTR) + strlen(CGISTR);
	char *arg[ARGNUM];
	int num = 0;
	char *rpath = wctl->conn.con_req.rpath;
	stat *fs = &wctl->conn.con_res.fsate;

	int retval = -1;
	/*查找CGI的命令*/
	char *pos = command;
	for(;*pos != '?' && *pos !='\0';pos++)
		;
	*pos = '\0';
	sprintf(rpath, "%s%s",conf_para.CGIRoot,command);

	/*CGI的参数*/
	pos++;
	for(;*pos != '\0' && num < ARGNUM;)
	{
		arg[num] = pos;
		for(;*pos != '+' && *pos!='\0';pos++)
			;
		if(*pos == '+')
		{
			*pos = '\0';
			pos++;
			num++;
		}
	}
	arg[num] = NULL;

	/*命令的属性*/
	if(stat(rpath,fs)<0)
	{
		/*错误*/
		res->status = 403;
		retval = -1;
		goto EXITcgiHandler;
	}
	else if((fs->st_mode & S_IFDIR) == S_IFDIR)
	{
		/*是一个目录,列出目录下的文件*/
		GenerateDirFile(wctl);
		retval = 0;
		goto EXITcgiHandler;
	} 
	else if((fs->st_mode & S_IXUSR) != S_IXUSR)
	{
		/*所指文件不能执行*/
		res->status = 403;
		retval = -1;
		goto EXITcgiHandler;
	}
	
	/*创建进程间通信的管道*/	
	int pipe_in[2];
	int pipe_out[2];

	if(pipe[pipe_in] < 0)
	{
		res->status = 500;
		retval = -1;
		goto EXITcgiHandler;
	}

	if(pipe[pipe_out] < 0)
	{
		res->status = 500;
		retval = -1;
		goto EXITcgiHandler;
	}

	/*进程分叉*/
	int pid = 0;
	pid = fork();
	if(pid < 0)/*错误*/
	{
		res->status = 500;
		retval = -1;
		goto EXITcgiHandler;
	}
	else if(pid > 0)/*父进程*/
	{
		close(pipe_out[WRITEOUT]);
		close(pipe_in[READIN]);

		{
			memset(path, 0, URI_MAX);

			size_t		n;
			unsigned long	r1, r2;
			char	*fmt = "%a, %d %b %Y %H:%M:%S GMT";

			/*需要确定的参数*/
			size_t status = 200;		/*状态值,已确定*/
			char *msg = "OK";	/*状态信息,已确定*/
			char  date[64] = "";		/*时间*/
			char  lm[64] = "";			/*请求文件最后修改信息*/
			char  etag[64] = "";		/*etag信息*/
			big_int_t cl;				/*内容长度*/
			char  range[64] = "";		/*范围*/	
			struct mine_type *mine = NULL;

			/*当前时间*/
			time_t t = time(NULL);	
			(void) strftime(date, 
						sizeof(date), 
						fmt, 
						localtime(&t));

			/*最后修改时间*/
			(void) strftime(lm, 
						sizeof(lm), 
						fmt, 
						localtime(&res->fsate.st_mtime));

			/*ETAG*/
			(void) snprintf(etag, 
						sizeof(etag), 
						"%lx.%lx",
						(unsigned long) res->fsate.st_mtime,
						(unsigned long) res->fsate.st_size);
			
			/*发送的MIME类型*/
			mine = Mine_Type(req->uri, strlen(req->uri), wctl);

			memset(res->res.ptr, 0, sizeof(wctl->conn.dres));
			snprintf(
				res->res.ptr,
				sizeof(wctl->conn.dres),
				"HTTP/1.1 %d %s\r\n" 
				"Date: %s\r\n"
				"Last-Modified: %s\r\n"
				"Etag: \"%s\"\r\n"
				"Content-Type: %s\r\n"
				"Accept-Ranges: bytes\r\n"		
				"%s\r\n",
				status,
				msg,
				date,
				lm,
				etag,
				"html",
				range);
			res->status = status;

			send(wctl->conn.cs, res->res.ptr, strlen(res->res.ptr));
		}

		int size = 0;
		int end = 0;
		while(size > 0 && !end)
		{
			size = read(pipe_out[READIN], res->res.ptr, sizeof(wctl->conn.dres));
			if(size > 0)
			{
				send(wctl->conn.cs, res->res.ptr, strlen(res->res.ptr));
			}
			else
			{
				end = 1;
			}
		}
		wait(&end);
		close(pipe_out[READIN]);
		close(pipe_in[WRITEOUT]);

		retval = 0;
	}
	else/*子进程*/
	{
		char cmdarg[2048];
		char onearg[2048];
		char *pos = NULL;
		int i = 0;

		/*形成执行命令*/
		memset(onearg, 0, 2048];
		for(i = 0;i<num;i++)
			sprintf(cmdarg,"%s %s", onearg, arg[i]);

		/*将写入的管道绑定到标注输出*/
		close(pipe_out[READIN]);    /* 关闭无用的读管道*/
		dup2(pipe_out[WRITEOUT], 1); /* 将写管道绑定到标注输出 */
		close(pipe_out[WRITEOUT]);   /* 关闭写管道 */

		close(pipe_in[WRITEOUT]);    
		dup2(pipe_in[READIN], 0);   
		close(pipe_in[READIN]);   

		/*执行命令,命令的输出需要为标准输出*/
		execlp(rpath,  arg);
       	  
	}
EXITcgiHandler:
	return retval;
}

