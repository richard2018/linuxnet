#include "shttpd.h"

extern void Error_400(struct worker_ctl* wctl);
extern void Error_403(struct worker_ctl* wctl);
extern void Error_404(struct worker_ctl* wctl);
extern void Error_505(struct worker_ctl* wctl);

static struct http_header http_headers[] = {
	{16,	HDR_INT,	OFFSET(cl),			"Content-Length: "		},
	{14,	HDR_STRING,	OFFSET(ct),			"Content-Type: "		},
	{12,	HDR_STRING,	OFFSET(useragent),	"User-Agent: "		},
	{19,	HDR_DATE,	OFFSET(ims),			"If-Modified-Since: "	},
	{15,	HDR_STRING,	OFFSET(auth),		"Authorization: "		},
	{9,	HDR_STRING,	OFFSET(referer),		"Referer: "			},
	{8,	HDR_STRING,	OFFSET(cookie),		"Cookie: "			},
	{10,	HDR_STRING,	OFFSET(location),		"Location: "			},
	{8,	HDR_INT,	OFFSET(status),		"Status: "				},
	{7,	HDR_STRING,	OFFSET(range),		"Range: "				},
	{12,	HDR_STRING,	OFFSET(connection),	"Connection: "			},
	{19,	HDR_STRING,	OFFSET(transenc),	"Transfer-Encoding: "	},
	{0,	HDR_INT,	0,					NULL				}
};
extern struct vec _shttpd_methods[];
struct conf_opts conf_para;

/*
 * Convert month to the month number. Return -1 on error, or month number
 */
static int
montoi(char *s)
{
	DBGPRINT("==>montoi\n");
	int retval = -1;
	static char *ar[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	size_t	i;

	for (i = 0; i < sizeof(ar) / sizeof(ar[0]); i++)
		if (!strcmp(s, ar[i])){
			retval = i;
			goto EXITmontoi;
		}
	DBGPRINT("<==montoi\n");
EXITmontoi:
	return retval;
}

/*
 * Parse date-time string, and return the corresponding time_t value
 */
static time_t
date_to_epoch(char *s)
{
	DBGPRINT("==>date_to_epoch\n");
	struct tm	tm;
	char		mon[32];
	int		sec, min, hour, mday, month, year;

	(void) memset(&tm, 0, sizeof(tm));
	sec = min = hour = mday = month = year = 0;

	if (((sscanf(s, "%d/%3s/%d %d:%d:%d",
	    &mday, mon, &year, &hour, &min, &sec) == 6) ||
	    (sscanf(s, "%d %3s %d %d:%d:%d",
	    &mday, mon, &year, &hour, &min, &sec) == 6) ||
	    (sscanf(s, "%*3s, %d %3s %d %d:%d:%d",
	    &mday, mon, &year, &hour, &min, &sec) == 6) ||
	    (sscanf(s, "%d-%3s-%d %d:%d:%d",
	    &mday, mon, &year, &hour, &min, &sec) == 6)) &&
	    (month = montoi(mon)) != -1) {
		tm.tm_mday	= mday;
		tm.tm_mon	= month;
		tm.tm_year	= year;
		tm.tm_hour	= hour;
		tm.tm_min	= min;
		tm.tm_sec	= sec;
	}

	if (tm.tm_year > 1900)
		tm.tm_year -= 1900;
	else if (tm.tm_year < 70)
		tm.tm_year += 100;

	DBGPRINT("<==date_to_epoch\n");
	return (mktime(&tm));
}

void Request_HeaderParse(char *s, int len, struct headers *parsed)
{
	DBGPRINT("==>Request_HeaderParse\n");
	struct http_header	*h;	/*结构struct http_header指针*/
	union variant			*v;/*通用参数*/
	char			*p, *e = s + len;/*p当前位置,e尾部*/


	/* 查找请求字符串中的头部关键字*/
	while (s < e) 
	{

		/* 查找一行末尾*/
		for (p = s; p < e && *p != '\n'; ) 
		{
			p++;
		}
		
		/* 已知方法*/
		for (h = http_headers; h->len != 0; h++)
		{
			if (e - s > h->len &&    /*字符串匹配*/
				!strncasecmp(s, h->name, h->len))
			{
				break;
			}
		}
		
		/* 将此方法放入*/
		if (h->len != 0) 
		{

			/* 请求字符串中值的位置*/
			s += h->len;

			/* 将值存放到参数parsed中 */
			v = (union variant *) ((char *) parsed + h->offset);

			/* 根据头部选项不同,计算不同的值*/
			if (h->type == HDR_STRING) /*字符串类型*/
			{
				v->v_vec.ptr = s;/*字符串开始*/
				v->v_vec.len = p - s;/*字符串长度*/
				if (p[-1] == '\r' && v->v_vec.len > 0)
				{
					v->v_vec.len--;
				}
			} 
			else if (h->type == HDR_INT) /*INT类型*/
			{
				v->v_big_int = strtoul(s, NULL, 10);
			} 
			else if (h->type == HDR_DATE) /*时间格式*/
			{
				v->v_time = date_to_epoch(s);
			}
		}

		s = p + 1;	/* Shift to the next header */
	}

	DBGPRINT("<==Request_HeaderParse\n");
}

#define JUMPOVER_CHAR(p,over) do{for(;*p== over;p++);}while(0);
#define JUMPTO_CHAR(p,to) do{for(;*p!= to;p++);}while(0);
/* 分析获得的请求 */
int  Request_Parse(struct worker_ctl *wctl)
{
	DBGPRINT("==>Request_Parse\n");
	struct worker_conn *c = &wctl->conn;
	struct conn_request *req = &c->con_req;
	struct conn_response *res = &c->con_res;
	int retval = 200;
	
	char *p = req->req.ptr;
	int len = req->req.len;
	char *pos = NULL;

	/*处理第一行*/
	/*[GET /root/default.html HTTP/1.1\r\n]*/
	/*查找一行末尾*/
	pos = memchr(p, '\n', len);
	if(*(pos-1) == '\r')
	{
		*(pos-1) = '\0';
	}
	*pos = '\0';

	pos = p;

	/***********************方法*********************/	
	int found = 0;
	
	JUMPOVER_CHAR(pos,' ');/*跳过空格*/
		;
	struct vec *m= NULL;
	/*查找比较方法字符串*/
	for(m = &_shttpd_methods[0];m->ptr!=NULL;m++)
	{
		if(!strncmp(m->ptr, pos, m->len))/*比较字符串*/
		{
			req->method = m->type;/*更新头部方法*/
			found = 1;
			break;
		}
	}
	if(!found)
	{
		retval = 400;
		goto EXITRequest_Parse;
	}

	/*URI分析*/
	pos += m->len;/*跳过方法*/
	JUMPOVER_CHAR(pos,' ');/*跳过空格*/

	len -= pos -p;
	p = pos;
	JUMPTO_CHAR(pos, ' ');/*跳到空格*/
	*pos = '\0';

	req->uri = (char*)p;
	/*文件*/
	snprintf(req->rpath, URI_MAX, "%s/%s",conf_para.DocumentRoot, req->uri);
	res->fd = open(req->rpath, O_RDONLY , 0644);
	if(res->fd != -1)
	{
		fstat(res->fd, &res->fsate);	
		if(S_ISDIR(res->fsate.st_mode))
		{
			retval = 403;
			goto EXITRequest_Parse;
		}
	}
	else
	{
		
		retval = 404;
		goto EXITRequest_Parse;
	}
		
	/*HTTP版本:
	*	HTTP/[1|0].[1|0|9]
	*/
	pos += 1;
	JUMPOVER_CHAR(pos,' ');/*跳过空格*/

	len -= pos -p;
	p = pos;
	sscanf(p,
		"HTTP/%lu.%lu",
		&req->major, 
		&req->minor);
	if(!((req->major == 0 && req->minor == 9)||
		(req->major == 1 && req->minor == 0)||
		(req->major == 1 && req->minor == 1)))
	{
		retval = 505;
		goto EXITRequest_Parse;
	}


	/*其他头部信息*/
	JUMPTO_CHAR(pos, '\0');
	JUMPOVER_CHAR(pos,'\0');/*跳过空字符*/
	len -= pos - p;
	p = pos;
	Request_HeaderParse(p, len, & req->ch);	
	DBGPRINT("URI:'%s',patch:'%s'\n",req->uri,req->rpath);
EXITRequest_Parse:
	DBGPRINT("<==Request_Parse\n");
	return retval;
}




int Request_Handle(struct worker_ctl* wctl)
{
	DBGPRINT("==>Request_Handle\n");
	int err = wctl->conn.con_req.err;		
	int cs = wctl->conn.cs;
	int cl = -1;
	char *ptr = wctl->conn.con_res.res.ptr;
	int len = -1;
	int n = -1;
	
	

	switch(err)
	{
		case 200:
			Method_Do(wctl);
			int fd = wctl->conn.con_res.fd;
			cl = wctl->conn.con_res.cl;
			len = strlen(wctl->conn.con_res.res.ptr);
			
			n = write(cs, ptr, len);
			printf("echo header:%s, write to client %d bytes, status:%d\n",ptr,n,wctl->conn.con_res.status);

			if(fd != -1)
			{		
				lseek(fd, 0, SEEK_SET);
				len = sizeof(wctl->conn.dres);
				printf("response len:%d, content length:%d\n",len,wctl->conn.con_res.cl);
				for(n = 0; cl>0; cl -= n)
				{
					n = read(fd,ptr,len>cl?cl:sizeof(wctl->conn.dres));
					printf("read %d bytes,",n);
					if(n > 0)
					{
						n =write(cs, ptr, n);
						printf("write %d bytes\n",n);
					}
				}
				close(fd);
				wctl->conn.con_res.fd = -1;
			}

			break;
		default:
		case 400:
			Error_400(wctl);
			cl = wctl->conn.con_res.cl;
			len = strlen(wctl->conn.con_res.res.ptr);
			
			n = write(cs, ptr, len);			
			break;	
		case 403:
			Error_403(wctl);
			cl = wctl->conn.con_res.cl;
			len = strlen(wctl->conn.con_res.res.ptr);
			
			n = write(cs, ptr, len);			
			break;	
		case 404:
			Error_404(wctl);
			cl = wctl->conn.con_res.cl;
			len = strlen(wctl->conn.con_res.res.ptr);
			
			n = write(cs, ptr, len);			
			break;	
		case 505:
			Error_505(wctl);
			cl = wctl->conn.con_res.cl;
			len = strlen(wctl->conn.con_res.res.ptr);
			
			n = write(cs, ptr, len);			
			break;	
		
	}

	DBGPRINT("<==Request_Handle\n");
	return 0;
}

