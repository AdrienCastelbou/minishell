#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "minishell.h"
#include <termios.h>
#include <term.h>
#include <curses.h>

struct termios saved_attributes;

void
reset_input_mode (void)
{
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}


void	set_mode(void)
{
	struct termios	t;
	int				r;

	tcgetattr (STDIN_FILENO, &saved_attributes);
	r = tcgetattr(STDIN_FILENO, &t);
	if (r)
	{
		printf("Oh no...\n");
		exit(0);
	}
	t.c_lflag &= ~(ICANON|ECHO);
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;
	r = tcsetattr(STDIN_FILENO, TCSANOW, &t);
	if (r)
	{
		printf("Oh no...\n");
		exit(0);
	}

}

int main( int argc, char ** argv )
{
	char	c;
	char	back[] = "\b \b";
	char	buff[3];
	int		top;
	char	*input;

	input = malloc(sizeof(char) * 1);
	*input = 0;
	top = 0;
	ft_bzero(buff, 3);
	set_mode();
	while (read (STDIN_FILENO, &c, 1))
	{
		if (c == '\004')
		{
			if (!*input && !*buff)
				break;
		}
		else if (c == '\n')          /* C-d */
			break;
		else if (c == 127)
		{
			if (!top && *input)
				input[ft_strlen(input) - 1] = 0;
			else if (top)
			{
				top -= 1;
				buff[top] = 0;
			}
			write(1, back, strlen(back));
		}
		else
		{
			buff[top] = c;
			write(1, &c, 1);
			top += 1;
			if (top >= 3 - 1)
			{
				input = ft_strjoin(input, buff);
				ft_bzero(buff, 3);
				top = 0;
			}
		}
	}
	reset_input_mode();
	input = ft_strjoin(input, buff);
	printf("\n%s\n", input);
  return EXIT_SUCCESS;
}
