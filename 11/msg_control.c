#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

ssize_t get_fd(int fd, void *ptr, size_t bytes, int *recvfd)
{
	struct msghdr msg;
	struct iovec iov[1];
	size_t n;
	int newfd;
	
	union{
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];	
	}control_un;
	struct cmsghdr *cmptr;
	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
	
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	
	iov[0].iov_base = ptr;
	iov[0].iov_len = bytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	if((n = recvmsg(fd, &msg, 0))<=0)
		return n;
		
	if((cmptr = CMSG_FIRSTHDR(&msg))!= NULL &&
		cmptr->cmsg_len == CMSG_LEN(sizeof(int))){
		if(cmptr->cmsg_level != SOL_SOCKET)
			printf("control level != SOL_SOCKET\n");
		
		if(cmptr->cmsg_type != SCM_RIGHTS)
			printf("control type != SCM_RIGHTS\n");
			
			*recvfd = *((int*)CMSG_DATA(cmptr));		
	}else
		*recvfd = -1;
		
	return n;
}



ssize_t put_fd(int fd, void *ptr, size_t bytes, int sendfd)
{
	struct msghdr msg;
	struct iovec iov[1];
	size_t n;
	int newfd;
	
	union{
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(int))];	
	}control_un;
	struct cmsghdr *cmptr;
	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
	
	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*((int*)CMSG_DATA(cmptr))= sendfd;
	
	
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	
	iov[0].iov_base = ptr;
	iov[0].iov_len = bytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	
	return (sendmsg(fd, &msg, 0));
}

