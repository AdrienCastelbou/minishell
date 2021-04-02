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
	begin->cmds->content = "ls";
	begin->cmds->next = NULL;
	begin->next = malloc(sizeof(t_cmdlst));
	begin->next->cmds = malloc(sizeof(t_list));
	begin->next->cmds->content = "wc";
	begin->next->cmds->next = NULL;
	begin->next->next = malloc(sizeof(t_cmdlst));
	begin->next->next->cmds = malloc(sizeof(t_list));
	begin->next->next->cmds->content = "cat";
	begin->next->next->next = NULL; 
	return (begin);
}

t_fds	*fd_creation(int fdn)
{
	t_fds	*fd;

	fd = malloc(sizeof(t_fds));
	fd->fd = fdn;
	fd->next = malloc(sizeof(t_fds));
	fd->next->fd = fdn;
	fd->next->next = malloc(sizeof(t_fds));
	fd->next->next->fd = fdn;
	fd->next->next->next = NULL;
	return (fd);
}

static void redirect(int oldfd, int newfd) {
	if (oldfd != newfd) {
	if (dup2(oldfd, newfd) != -1)
		close(oldfd); /* successfully redirected */
	else
		;
  }
}

void	run(char *content, int in, int out)
{
	redirect(in, STDIN_FILENO);   /* <&in  : child reads from in */
	redirect(out, STDOUT_FILENO);
	execlp(content, content, (char *) 0);
}

void	make_pipe(t_cmdlst *lst, t_fds *in, t_fds *out)
{
	int fd[2];
	int pid;
	int	fin;
	int	fout;

	fout = 1;
	fin = 0;
	while (lst->next)
	{
		if (in->fd != 0)
			fin = in->fd;
		if (pipe(fd) == -1)
			return ;
		else if ((pid = fork()) < 0)
			return ;
		if (pid == 0)
		{
			fout = fd[1];
			if (out->fd != 1)
				fout = out->fd;
			// CHILD PROCESS
			close(fd[0]);
			run(lst->cmds->content, fin, fout);
		}
		else
		{
			close(fd[1]);

			close(fin);
			fin = fd[0];
		}
		lst = lst->next;
		in = in->next;
		out = out->next;
	}
	if (in && in->fd != 0)
			fin = in->fd;
	else
		fin = 0;
	if (out && out->fd != 1)
		fout = out->fd;
	else
		fout = 1;
	run(lst->cmds->content, fin, fout);
}

int main( int argc, char ** argv )
  {
	t_cmdlst	*lst;
	t_fds		*fd_in;
	t_fds		*fd_out;
	int			fd;

	fd = open("file", O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
	int	f = open("test.c", O_RDONLY);
	int	fdd = open("minishell.h", O_RDONLY);
	lst = create_lst();
	fd_in = fd_creation(0);
	fd_in->next->fd = f;
	fd_out = fd_creation(1);
	fd_out->next->fd = fd;
	fd_in->next->next->fd = f;

   /* create the pipe */
   int pfd[2];
	make_pipe(lst, fd_in, fd_out);
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
