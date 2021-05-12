NAME=	minishell
SRCS=	main.c historic.c prompt.c prompt_utils.c set_term.c instructions.c \
		pipe.c run_cmd.c fds.c echo_builtin.c exit_builtin.c \
		cd_builtin.c unset_builtin.c
OBJS=	$(addprefix srcs/, $(SRCS:.c=.o))
CC=		gcc
CFLAGS=	-Wextra -Wall -Werror
LIBFT_DIR= libft
HEADER=	-I includes -I $(LIBFT_DIR)
LDFLAGS=	-L $(LIBFT_DIR)
LIBS=	-lft

.c.o:
	$(CC) $(CFLAGS) $(HEADER) -c $< -o $(<:.c=.o)
$(NAME):	$(OBJS)
	make bonus -C $(LIBFT_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(NAME) $(LIBS) -lncurses
all: $(NAME)
clean:
	rm -f $(OBJS)
fclean: clean
	rm -f $(NAME)
re: fclean all
.PHONY: all clean fclean re
