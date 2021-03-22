#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

typedef struct s_lstfd {
		int		fd;
		struct s_lstfd	*next;
}		t_lstfd;

t_lstfd	*create_fd(char *s)
{
	t_lstfd *elem;

	elem = malloc(sizeof(t_lstfd));
	elem->fd = open(s, O_WRONLY | O_TRUNC | O_CREAT, 777);
	elem->next = NULL;
	return (elem);
}

int main( int ac, char ** av)
{
   /* create the pipe */
	int	pfd[2];
	int	fd;	
	int	i;

	fd = 1;
	t_lstfd *lst;
	t_lstfd *elem;
	i = 1;
	lst = malloc(sizeof(t_lstfd));
	lst->fd = 1;
	lst->next = NULL;
	lst->next = create_fd(av[i]);
	elem = lst->next;
	while (++i <= ac)
	{
		elem->next = create_fd(av[i]);
		elem = elem->next;
	}

	// CREATION LISTE FD OK
	
	elem = lst;
	while (elem && elem->next)
	{
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
		elem = lst;
		if (pid == 0)
		{
			/* child */
			char	buff[128];
			close(pfd[1]); /* close the unused write side */
			dup2(pfd[0], 0); /* connect the read side with stdin */
			int s = read(pfd[0], buff, 127);
			write(fd, buff, s);
			close(pfd[0]); /* close the read side */
			/* execute the process (wc command) */
			return 3;
		}
		else
		{
			/* parent */
			close(pfd[0]); /* close the unused read side */
			dup2(pfd[1], elem->fd); /* connect the write side with stdout */
			close(pfd[1]); /* close the write side */
			/* execute the process (ls command) */
			write(elem->fd, "hello\n", 5);
			return 4;
		}
	}
	return 0;
  }
