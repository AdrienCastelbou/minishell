#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "minishell.h"
#include <termios.h>
#include <term.h>
#include <curses.h>

int	pid;

void	hand(int s)
{
	kill(pid, SIGQUIT);
}

int main(void)
{
	pid  = -1;
	signal(SIGQUIT, hand);
	while (1)
	{
	printf("turn\n");
	pid = fork();
	if (pid > 0)
	{
		wait(NULL);
	}
	else
	{
		execlp("cat", "cat", NULL);
	}
	}
}
