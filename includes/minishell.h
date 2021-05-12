
#ifndef MINISHELL_H

#define MINISHELL_H
#include "libft.h"
#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <term.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <signal.h>
#include <wait.h>
#include <sys/ioctl.h>

typedef struct	s_files_portal {
		int	pfd[2];
		int	fdin;
		int	fdout;
}				t_files_portal;

typedef struct	s_cursor {
		int	cur_col;
		int	cur_line;
}				t_cursor;

typedef struct	s_fds {
		char			*name;
		char			*method;
		int				fd;
		int				is_file;
		struct s_fds	*next;
}				t_fds;

typedef struct	s_history {
		char				*input;
		int					is_prompt;
		struct s_history	*previous;
		struct s_history	*next;
}				t_history;

typedef struct	s_instructions {
		int						is_empty;
		t_list					*cmds;
		t_fds					fdin;
		t_fds					fdout;
		struct s_instructions	*next;
}				t_instructions;

typedef struct	s_mini {
		char		**cmd;
		char			**envp;
		char			*input;
		char			*to_exec;
		char			*bin;
		char			*path_list;
		struct	winsize	ws;
		t_instructions	*instructions;
		t_cursor		cursor;
		t_history		*history;
		t_history		*current_hist;
		t_list			*cmds;
		t_list			*env;
		int				stdin_copy;
		int				stdout_copy;
		int				is_pipe;
		int				last_return;
		char			*cm_cap;
		char			*dc_cap;
		char			*le_cap;
		char			*ce_cap;
}				t_mini;

typedef struct	s_sigcatch {
		int		should_run;
		int		pid;
		struct termios	saved_attributes;
}		t_sigcatch;

t_sigcatch	sig_catcher;

//FREE
void			ft_free_splited(char **splited_inputs);
void			free_inputs(t_mini *mini);
void			free_cmds(t_mini *mini);

//BUILTINS
int				exit_minishell(char	**splited_inputs, t_mini *mini);
int				echo_builtin(char	**splited_inputs);
int				env_builtin(t_list *env);
void			add_env_var(char *env_var, t_list *env, char *key);
int				export_builtin(t_mini *mini, char	**splited_inputs, t_list *env);
void			delete_env_var(char *key, t_list **env);
int				unset_builtin(t_mini *mini, char	**splited_inputs, t_list *env);
int				pwd_builtin(void);
int				cd_builtin(t_mini *mini, char *mov, char **inputs);
int				run_builtins(char	**splited_inputs, t_mini *mini);

//PARSING
int		ft_word_size(const char *s);
int				ft_isenvchar(char c);
int				jump_quotes_in_parsing(char *input, char quote, int *i);
char			*get_env_var(const char *s, t_list *env);
char			*join_input_parts(char *s, char *new, int i);
char			*get_var_value(char *s, char *new, int i, t_list *env);
char			*update_input_with_var(char **s, char *new, int *i, t_list *env);
char			*update_input_with_big_quotes(char **s, char *new, int *i, t_mini *mini);
char			*update_input_with_lil_quotes(char **s, char *new, int *i);
char			*update_input_with_echap(char **s, char *new, int *i);
char			*update_input_with_last_return(char **s, char *new, int*i, t_mini *mini);
char			*get_real_input(char *s, t_mini *mini, t_list *env);
int				cmd_count(char *input);
t_list			*ft_lst_input(t_mini *mini, t_instructions *instruc, char *s);
t_list			*ft_lst_cmds(t_mini *mini, char *s);

//RUN CMDS

int				run_bin(char **cmd, t_mini *mini, char *path_list);

//TERM MODE
int		set_mode(void);
int		reset_input_mode (void);
//PROMPT READER

int		read_prompt(t_mini *mini);
int		check_prompt_input(t_mini *mini, int *top, char *buffchar, char *buff);
int		check_input_validity(t_mini *mini, char *buffchar, int *top, char *buff);
int		is_arrow(char *buff);
void			get_cursor_position(int *col, int *line);
int				ft_putchar(int c);
int		write_char_in_prompt(t_mini *mini, char c, int *top, char *buff);
int		erase_char_in_prompt(t_mini *mini, int *top, char *buff);
int		join_prompt_parts(t_mini *mini, char *buff);
void	ft_history_add_front(t_history **ahist, t_history *new);
t_history	*ft_historynew(void);

//HISTORICS FCTNS

void	erase_current_line(t_mini *mini, t_cursor *curs);
void	erase_current_prompt(t_mini *mini, int *top, char *buff, t_cursor *curs);
int		add_input_in_history(t_mini *mini);
int		up_history(t_mini *mini, int *top, char *buff, t_cursor *cursor);
int		down_history(t_mini *mini, int *top, char *buff, t_cursor *cursor);

//UTILS

char			*ft_strjoin_path(char const *s1, char const *s2);
char			**transform_env_lst_in_tab(t_list *env);
void			set_mini(t_mini *mini);
void			get_cmds_tab(t_mini *mini);
void			run_cmd(t_mini *mini, char **cmd, t_instructions *instruc);
int				ft_get_input(t_mini *mini);
t_list			*copy_env(char **envp);
t_mini			*init_mini(t_list *env);
int				ft_strcmp(char *s1, char *s2);
char			*ft_strndup(char *s, int len);


// FREE

void	ft_instruclear(t_instructions **instruc);
#endif
