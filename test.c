#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "minishell.h"

typedef struct		s_cmdlst {
		t_list		*cmds;
		struct s_cmdlst	*next;
}					t_cmdlst;

t_cmdlst *create_lst(void)
{
	t_cmdlst	*begin;

	begin = malloc(sizeof(t_cmdlst));
	begin->cmds = malloc(sizeof(t_list));
	begin->cmds->content = "wc";
	begin->cmds->next = NULL;
	begin->next = malloc(sizeof(t_cmdlst));
	begin->next->cmds = malloc(sizeof(t_list));
	begin->next->cmds->content = "ls";
	begin->next->cmds->next = NULL;
	begin->next->next = NULL;
	return (begin);
}

t_fds	*fd_creation(int fdn)
{
	t_fds	*fd;

	fd = malloc(sizeof(t_fds));
	fd->fd = fdn;
	fd->next = malloc(sizeof(t_fds));
	fd->next->fd = fdn;
	fd->next->next = NULL;
	return (fd);
}

void	make_pipe(t_cmdlst *lst, t_fds *in, t_fds *out, int piped[])
{
	int	pid;
	int	status;
	int	pfd[2];


	printf("%s\n", lst->cmds->content);
	if (lst->next)
		pipe(pfd);
	pid = fork();
	if (pid == 0)
	{
		if (lst->next)
		{
			close(pfd[1]);
			//pqssqge en mode lecture pour recevoir son output
			dup2(pfd[0], in->fd);
			make_pipe(lst->next, in->next, out->next, pfd);
			close(0);
		}
		//printf("ueue\n");
		if(piped[0] != -1)
		{
			close(piped[0]);
			dup2(piped[1], 1);
close(piped[1]);
		}
		//printf("ueue\n");
		execlp(lst->cmds->content, lst->cmds->content, (char *) 0);
		return ;
	}
	else
	{
		wait(&status);
		return;
	}
}

int main( int argc, char ** argv )
  {
	t_cmdlst	*lst;
	t_fds		*fd_in;
	t_fds		*fd_out;

	lst = create_lst();
	fd_in = fd_creation(0);
	fd_out = fd_creation(1);
   /* create the pipe */
   int pfd[2];
   *pfd = -1;
	make_pipe(lst, fd_in, fd_out, pfd);
	return (1);
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
       dup2(pfd[0], fd_in->next->fd); /* connect the read side with stdin */
       close(pfd[0]); /* close the read side */
       /* execute the process (wc command) */
       execlp(lst->next->cmds->content, lst->next->cmds->content, (char *) 0);
       printf("wc failed"); /* if execlp returns, it's an error */
       return 3;
     }
   else
     {
       /* parent */
       close(pfd[0]); /* close the unused read side */
       dup2(pfd[1], fd_out->fd); /* connect the write side with stdout */
       close(pfd[1]); /* close the write side */
       /* execute the process (ls command) */
       execlp(lst->cmds->content, lst->cmds->content, (char *)0);
       printf("ls failed"); /* if execlp returns, it's an error */
       return 4;
     }
   printf("ya\n");
   return 0;
  }
