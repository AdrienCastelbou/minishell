#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "minishell.h"
#include <termios.h>
#include <term.h>
#include <curses.h>

int	init_term()
{
	int ret;
	char *term_type = getenv("TERM");

	if (term_type == NULL)
	{
		fprintf(stderr, "TERM must be set (see 'env').\n");
		return -1;
	}
	 ret = tgetent(NULL, term_type);
	if (ret == -1)
	{
		fprintf(stderr, "Could not access to the termcap database..\n");
		return -1;
	}
	else if (ret == 0)
	{
		fprintf(stderr, "Terminal type '%s' is not defined in termcap database (or have too few informations).\n", term_type);
		return -1;
	}
	return 0;
}

int		ft_putchar(int c)
{
	write(1, &c, 1);
	return (1);
}

int	main(void)
{
	int ret = init_term();
	if (ret != 0)
		return (0);
	int col = tgetnum("co");
	int line = tgetnum("li");
	printf("col = %d, li = %d", col, line);
	char *cm_cap = tgetstr("cm", NULL);
	cm_cap = tgetstr("dc", NULL);
	tputs(cm_cap, 1, putchar);
tputs(cm_cap, 1, putchar);
	char *reset_cmd = tgetstr("me", NULL);
tputs(reset_cmd, 1, putchar);
}
