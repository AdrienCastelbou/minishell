#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <term.h>
#include <unistd.h>
#include "libft.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>

typedef struct	s_fds {
		int			fd;
		struct s_fds	*next;
}				t_fds;

typedef struct	s_mini {
		char	**cmd;
		char	**envp;
		char	*input;
		t_list	*cmds;
		t_list	*env;
		t_fds	*fds;
		int		current_fd;
}				t_mini;

//FREE
void			ft_free_splited(char **splited_inputs);
void			free_inputs(t_mini *mini);
void			free_cmds(t_mini *mini);

//BUILTINS
int				exit_minishell(char	**splited_inputs, t_mini *mini);
int				echo_builtin(char	**splited_inputs);
int				env_builtin(t_list *env);
void			add_env_var(char *env_var, t_list *env, char *key);
int				export_builtin(char	**splited_inputs, t_list *env);
void			delete_env_var(char *key, t_list **env);
int				unset_builtin(char	**splited_inputs, t_list *env);
int				pwd_builtin(void);
int				cd_builtin(t_mini *mini, char *mov);
int				run_builtins(char	**splited_inputs, t_mini *mini);

//PARSING
static int		ft_word_size(const char *s, char c);
static int		ft_words_count(char const *s, char c);
char			*get_env_var(const char *s, t_list *env);
char			*join_input_parts(char *s, char *new, int i);
char			*get_var_value(char *s, char *new, int i, t_list *env);
char			*update_input_with_var(char **s, char *new, int *i, t_list *env);
char			*update_input_with_big_quotes(char **s, char *new, int *i, t_list *env);
char			*update_input_with_lil_quotes(char **s, char *new, int *i, t_list *env);
char			*get_real_input(char *s, t_list *env);
int				cmd_count(char *input);
t_list			*ft_lst_input(t_mini *mini, char *s, char c, t_list *env);
t_list			*ft_lst_cmds(t_mini *mini, char *s, t_list *env);

//UTILS
char			**transform_env_lst_in_tab(t_list *env);
void			set_mini(t_mini *mini);
void			get_cmds_tab(t_mini *mini);
void			run_cmd(t_mini *mini, char **cmd);
int				ft_get_input(t_mini *mini);
t_list			*copy_env(char **envp);
t_mini			*init_mini(char **envp_tocpy);
int				ft_strcmp(char *s1, char *s2);
