

int flags = -1;
int err = -1;
flags = fcntl(s, F_GETFL, 0);
if(flags < 0){
	printf("fcntl F_GETFL ERROR\n");	
}
if(!(flags | NON_BLOCK))
{
	flags |= NON_BLOCK;
	err = fcntl(s, F_SETFL, flags);
	if(err < 0){
		printf("fcntl F_SETFL ERROR\n");	
	}
}else{
	printf("socket %d already set to NON_BLOCK\n",s);	
}

int flags = NON_BLOCK;
err = fcntl(s, F_SETFL, flags);