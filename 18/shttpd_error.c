#include "shttpd.h"

enum{
	ERROR301, ERROR302, ERROR303, ERROR304, ERROR305, ERROR307,
	ERROR400, ERROR401, ERROR402, ERROR403, ERROR404, ERROR405, ERROR406, 
	ERROR407, ERROR408, ERROR409, ERROR410, ERROR411, ERROR412, ERROR413,
	ERROR414, ERROR415, ERROR416, ERROR417,
	ERROR500, ERROR501, ERROR502, ERROR503, ERROR504, ERROR505
};

struct error_mine{
	int error_code;
	char *content;
	char *msg;
	int status;
};
struct error_mine _error_http[]=
{
	{ERROR301, "Error: 301", "永久移动", 		301},
	{ERROR302, "Error: 302", "创建", 				302},
	{ERROR303, "Error: 303", "观察别的部分", 	303},
	{ERROR304, "Error: 304", "只读", 				304},
	{ERROR305, "Error: 305", "用户代理", 		305},
	{ERROR307, "Error: 307", "临时重发", 		307},	
	
	{ERROR400, "Error: 400", "坏请求", 			400},
	{ERROR401, "Error: 401", "未授权的", 		401},
	{ERROR402, "Error: 402", "必要的支付", 	402},
	{ERROR403, "Error: 403", "禁用", 				403},
	{ERROR404, "Error: 404", "没找到", 			404},
	{ERROR405, "Error: 405", "不允许的方式", 	405},
	{ERROR406, "Error: 406", "不接受", 			406},
	{ERROR407, "Error: 407", "需要代理验证", 	407},
	{ERROR408, "Error: 408", "请求超时", 		408},
	{ERROR409, "Error: 409", "冲突", 				409},
	{ERROR410, "Error: 410", "停止", 				410},
	{ERROR411, "Error: 411", "需要的长度", 	411},
	{ERROR412, "Error: 412", "预处理失败", 	412},
	{ERROR413, "Error: 413", "请求实体太大", 	413},
	{ERROR414, "Error: 414", "请求-URI太大", 	414},
	{ERROR415, "Error: 415", "不支持的媒体类型", 415},
	{ERROR416, "Error: 416", "请求的范围不满足", 416},
	{ERROR417, "Error: 417", "期望失败", 		417},
	
	{ERROR500, "Error: 500", "服务器内部错误", 500},
	{ERROR501, "Error: 501", "不能实现", 		501},
	{ERROR502, "Error: 502", "坏网关", 			502},
	{ERROR503, "Error: 503", "服务不能实现", 	503},
	{ERROR504, "Error: 504", "网关超时", 		504},
	{ERROR505, "Error: 505", "HTTP版本不支持", 505}
};

void Error_400(struct worker_ctl* wctl)
{
	;
}

void Error_403(struct worker_ctl* wctl)
{
	;
}

void Error_404(struct worker_ctl* wctl)
{
	;
}

void Error_505(struct worker_ctl* wctl)
{
	;
}

int GenerateErrorMine(struct worker_ctl * wctl)
{
	struct error_mine *err = NULL;
	int i = 0;
	for(err = &_error_http[i]; 
		err->status != wctl->conn.con_res.status;
		i++)
		;
	if(err->status != wctl->conn.con_res.status)
	{
		err = &_error_http[0]; 
	}

	snprintf(
		wctl->conn.dres,
		sizeof(wctl->conn.dres),
		"HTTP/%lu.%lu %d %s\r\n"
		"Content-Type:%s\r\n"
		"Content-Length:%d\r\n"
		"\r\n"
		"%s",
		wctl->conn.con_req.major,
		wctl->conn.con_req.minor,
		err->status,
		err->msg,
		"text/plain",
		strlen(err->content),
		err->content);

	wctl->conn.con_res.cl = strlen(err->content);
	wctl->conn.con_res.fd = -1;
	wctl->conn.con_res.status = 400;
	
	return 0;
}

