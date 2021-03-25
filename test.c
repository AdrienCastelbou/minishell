#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main( int argc, char ** argv )
  {
	char buff[128];
	int fd = open("file", O_RDONLY);
   /* create the pipe */
   int pfd[2];
   if (pipe(pfd) == -1)
     {
       printf("pipe failed\n");
       return 1;
     }

   /* create the child */
   int pid;
   if ((pid = fork()) < 0)
     {
       printf("fork failed\n");
       return 2;
     }
 
   if (pid == 0)
     {
       /* child */
       close(pfd[1]); /* close the unused write side */
       dup2(pfd[0], 0); /* connect the read side with stdin */
	   dup2(fd, 0);
       close(pfd[0]); /* close the read side */
	int size = read(0, buff, 128);
	write(1, buff, size);
       /* execute the process (wc command) */
       execlp("cat", "cat", (char *) 0);
       printf("wc failed"); /* if execlp returns, it's an error */
       return 3;
     }
   else
     {
       /* parent */
       close(pfd[0]); /* close the unused read side */
       dup2(pfd[1], 1); /* connect the write side with stdout */
       //close(pfd[1]); /* close the write side */
		printf("file");
       /* execute the process (ls command) */
       //execlp("ls", "ls", (char *)0);
       //printf("file"); /* if execlp returns, it's an error */
		close(pfd[1]);
	   return 4;
     }
   return 0;
  }
