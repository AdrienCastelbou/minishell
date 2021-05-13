NAME=	minishell
SRCS=	main.c historic.c prompt.c prompt_utils.c set_term.c instructions.c \
		pipe.c run_cmd.c run_cmd_utils.c fds.c echo_builtin.c exit_builtin.c \
		cd_builtin.c unset_builtin.c export_builtin.c \
		env_builtin.c pwd_builtin.c get_real_input.c \
		get_real_input_utils.c get_tokens_size.c \
		ft_free.c ft_free_utils.c errors.c history_utils.c ft_utils.c \
		parsing.c parsing_utils.c set_mini.c
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
