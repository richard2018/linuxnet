

#define YES 1
#define NO 0

int s;
int err;
int optval = YES;
s = socket(AF_INET, SOCK_DGRAM,0);
err = setsockopt(
		s, 
		SOL_SOCKET, 
		SO_BROADCAST, 
		&optval, 
		sizeof(optval));
if(err)
	perror("setsockopt");
	
	
#define YES 1
#define NO 0
int s;
int err;
int optval = YES;
err = setsockopt(
	s,
    SOL_SOCKET,
    SO_KEEPALIVE,
    &optval,
    sizeof(optval));
if(err)
	perror("setsockopt");
	

	
#define YES 1
#define NO 0
int s;
int err;
struct linger optval;
optval.l_onoff = YES;
optval.l_linger = 60;
err = setsockopt(
	s,
    SOL_SOCKET,
    SO_LINGER,
    &optval,
    sizeof(optval));
if(err)
	perror("setsockopt");

#define YES 1
#define NO 0
int s;
int err;
struct linger optval;
optval.l_onoff = YES;
optval.l_linger = 0;
err = setsockopt(
	s,
    SOL_SOCKET,
    SO_LINGER,
    &optval,
    sizeof(optval));
if(err)
	perror("setsockopt");

#define YES 1
#define NO 0

int s;
int err;
int optval = YES;

err = setsockopt(
		s, 
		SOL_SOCKET, 
		SO_REUSEADDR, 
		&optval, 
		sizeof(optval));
if(err)
	perror("setsockopt");


#define YES 1
#define NO 0

int s;
int err;
int optval = YES;

err = setsockopt(
		s, 
		SOL_SOCKET, 
		SO_REUSEADDR, 
		&SO_OOBINLINE, 
		sizeof(optval));
if(err)
	perror("setsockopt");	
	
