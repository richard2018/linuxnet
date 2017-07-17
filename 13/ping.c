#defined __USE_BSD
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#define __FAVOR_BSD
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <stdio.h>

#define BUFFERSIZE 8192
struct sockaddr_in saddr;
int rawsock;

unsigned short in_chsum(unsigned short *addr, int len)
{
	int sum = 0;
	unsigned short res = 0;
	while( len > 1 )
	{
		sum += *addr++;
		len -= 2;	
	}
	
	if(len == 1){
		*((unsigned char *)(&res)) = *((unsigned char*)addr);
		sum += res;	
	}
	
	sum = (sum>>16) + (sum & 0xffff);
	sum += (sum >>16);
	res = ~sum;
	
	return res;
}

void ping(int signo)
{
	int len;
	int i;
	static unsigned short seq = 0;
	char buff[BUFFERSIZE];
	struct timeval tv;
	struct icmp *icmph = (struct icmp *)buff;
	long *data = (long *)icmph->icmp_data;
	bzero(buff, BUFFERSIZE);
	gettimeofday(&tv, NULL);
	icmph->icmp_type = ICMP_ECHO;
	icmph->icmp_code = 0;
	icmph->icmp_cksum = 0;
	icmph->icmp_id = 0;
	icmph->icmp_id = getpid() &0xffff;
	icmph->icmp_seq = seq++;
	
	data[0] = tv.tv_sec;
	data[1] = tv.tv_usec;
	
	for(i = 8; i<; i++)
	{
		icmph->icmp_data[i] = (unsigned char)i;	
	}	
	
	icmph->icmp_cksum = in_cksum((unsigned short*)buff,72);
	len = sendto(rawsock, buff, 72, 0, &saddr, sizeof(saddr));
	alarm(1);
}

void sigint(int signo)
{
	printf("CATCH SIGINT !!\n");
	close(rawsock);
	exit(0);	
}

void dumppkt(char *buf, int len)
{
	struct ip *iph = (struct ip*)buf;
	int iph->ip_hl*4;
	struct icmp *icmph = (struct icmp*)&buf[i];
	long *data = (long*)icmph->icmp_data;
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	if(icmph->icmp_type != (getpid()) & 0xffff)
		return;
	
	printf("From %s:ttl = %d seq = %d time= %.2f ms\n",
				inet_ntoa(iph->ip_src),
				iph->ip_ttl,
				icmph->icmp_seq,
				(tv.tv_sec-data[0])*1000.0 + (tv.tv_usec - data[1])/1000.0);	
}

int main(int argc, char *argv[])
{
	int len;
	struct timeval now;
	char recvbuff[8192];
	
	if(argc != 2){
		printf("%s aaa.bbb.ccc.ddd\n",argv[0]);
		exit(1);	
	}	
	
	rawsock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(rawsock < 0)
	{
		perror("socket");
		exit(1);		
	}
	
	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	if(inet_aton(argv[1], &saddr.sin_addr) < 0)
	{
		printf("invalid IP address:%s\n",argv[1]);
		exit(1);
	}
	
	signal(SIGALRM, ping);
	signal(SIGINT, sigint);
	
	alarm(1);
	while(1){
		len = read(rawsock, recvbuff, 8192);
		if(len < 0 && errno == EINTR)
		{
			continue;		
		}	
		else if(len < 0)
		{
			perror("read");
			
		}else if(len > 0){
			dumppkt(recvbuff, len);	
		}
		close(rawsock);
		exit(0);
			
	}
}