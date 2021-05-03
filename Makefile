NAME=	minishell
SRCS=	main.c
OBJS=	$(addprefix srcs/, $(SRCS:.c=.o))
CC=		gcc
CFLAGS=	-lncurses 
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
