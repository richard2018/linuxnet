	intfd, cork = 1; 
	… 
	/* 初始化*/ 
	… 
	setsockopt (fd, SOL_TCP, TCP_CORK, &cork, sizeof (cork)); /* 用水桶接水 */ 
	write (fd, …); 
	senddata (fd, …); 
	.../* 其他处理 */
	write (fd, …); 
	senddata (fd, …); 
	… 
	cork = 0; 
	setsockopt (fd, SOL_TCP, TCP_CORK, &on, sizeof (on)); /* 拿走水桶 */ 
