#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "minishell.h"

int main( int argc, char ** argv )
	{
		char *str;
		str = "hello";
		str[8] = 'a';
		return (1);
	}
