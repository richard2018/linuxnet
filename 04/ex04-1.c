/*
NAME
       pipe - create pipe

SYNOPSIS
       #include <unistd.h>

       int pipe(int filedes[2]);

DESCRIPTION
       pipe()  creates  a  pair of file descriptors, pointing to a pipe inode,
       and places them in the array pointed to by filedes.  filedes[0] is  for
       reading, filedes[1] is for writing.

RETURN VALUE
       On  success,  zero is returned.  On error, -1 is returned, and errno is
       set appropriately.

ERRORS
       EFAULT filedes is not valid.

       EMFILE Too many file descriptors are in use by the process.

       ENFILE The system limit on the total number  of  open  files  has  been
              reached.

CONFORMING TO
       POSIX.1-2001.

EXAMPLE
       The  following  program  creates  a pipe, and then fork(2)s to create a
       child process.  After the fork(2), each process closes the  descriptors
       that  it  doesn¡¯t  need  for  the  pipe (see pipe(7)).  The parent then
       writes the string contained in the program¡¯s command-line  argument  to
       the  pipe,  and  the  child reads this string a byte at a time from the
       pipe and echoes it on standard output.

       #include <sys/wait.h>
       #include <assert.h>
       #include <stdio.h>
       #include <stdlib.h>
       #include <unistd.h>
       #include <string.h>

       int
       main(int argc, char *argv[])
       {
           int pfd[2];
           pid_t cpid;
           char buf;

           assert(argc == 2);

           if (pipe(pfd) == -1) { perror("pipe"); exit(EXIT_FAILURE); }

           cpid = fork();
           if (cpid == -1) { perror("fork"); exit(EXIT_FAILURE); }

           if (cpid == 0) {    
               close(pfd[1]);         
               while (read(pfd[0], &buf, 1) > 0)
                   write(STDOUT_FILENO, &buf, 1);

               write(STDOUT_FILENO, "\n", 1);
               close(pfd[0]);
               _exit(EXIT_SUCCESS);

           } else {           
               close(pfd[0]);         
               write(pfd[1], argv[1], strlen(argv[1]));
               close(pfd[1]);         
               wait(NULL);            
               exit(EXIT_SUCCESS);
           }
       }

SEE ALSO
       fork(2), read(2), socketpair(2), write(2), popen(3), pipe(7)

*/

#include <sys/wait.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int
main(int argc, char *argv[])
{
	int pfd[2];
	pid_t cpid;
	char buf;
	
	assert(argc == 2);
	if (pipe(pfd) == -1) 
	{ 
		perror("pipe"); 
		exit(EXIT_FAILURE); 
	}
	
	cpid = fork();
	if (cpid == -1) 
	{ 
		perror("fork"); 
		exit(EXIT_FAILURE); 
	}
	
	if (cpid == 0) 
	{    
		close(pfd[1]);
		while (read(pfd[0], &buf, 1) > 0)
		write(STDOUT_FILENO, &buf, 1);
		
		write(STDOUT_FILENO, "\n", 1);
		close(pfd[0]);
		_exit(EXIT_SUCCESS);
	} 
	else 
	{    
		close(pfd[0]);
		write(pfd[1], argv[1], strlen(argv[1]));
		close(pfd[1]);
		wait(NULL);    
		exit(EXIT_SUCCESS);
	}
}