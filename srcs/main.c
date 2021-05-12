#include "minishell.h"

char	*ft_strndup(char *src, int size)
{
	int		i;
	char	*str;

	if (src == NULL)
		return (NULL);
	if ((str = malloc(sizeof(char) * size + 1)) == NULL)
		return (NULL);
	i = -1;
	while (++i < size)
		str[i] = src[i];
	str[i] = '\0';
	return (str);
}
int		ft_strcmp(char *s1, char *s2)
{
	while (*s1 && *s2)
	{
		if (*s1 !=  *s2)
			return (*s1 - *s2);
		s1++;
		s2++;
	}
	return (*s1 - *s2);
}

void	ft_free_splited(char **splited_inputs)
{
	int i;
	i = -1;
	while (splited_inputs[++i])
		free(splited_inputs[i]);
	free(splited_inputs);
}

void	free_mini_cmd(t_mini *mini)
{
	int i;

	i = -1;
	if (mini->cmd)
	{
		while (mini->cmd[++i])
		{
			free(mini->cmd[i]);
			mini->cmd[i] = NULL;
		}
		free(mini->cmd);
		mini->cmd = NULL;
	}
}

void	free_mini_tab(char **strs)
{
	int i;

	i = -1;
	if (strs)
	{
		while (strs[++i])
		{
			free(strs[i]);
			strs[i] = NULL;
		}
		free(strs);
		strs = NULL;
	}
}

void	free_inputs(t_mini *mini)
{
	if (!mini)
		return ;
	if (mini->cmd)
		free_mini_tab(mini->cmd);
	mini->cmd = NULL;
	if (mini->input)
		free(mini->input);
	mini->input = NULL;
}

void	free_history(t_history **elem)
{
	if (!elem || !*elem)
		return ;
	if ((*elem)->next)
		free_history(&(*elem)->next);
	if ((*elem)->input)
	{
		free((*elem)->input);
		(*elem)->input = NULL;
	}
	free((*elem));
	*elem = NULL;

}

int		print_errors(char *cmd, char *error, char *more, int nb)
{
	ft_putstr_fd("\U0000274C minishell: ", STDERR_FILENO);
	if (cmd && *cmd)
	{
		ft_putstr_fd(cmd, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	if (cmd && (!ft_strcmp("export", cmd) || !ft_strcmp("unset", cmd)))
		ft_putchar_fd('\'', STDERR_FILENO);
	if (error && *error)
		ft_putstr_fd(error, STDERR_FILENO);
	if (cmd && (!ft_strcmp("export", cmd) || !ft_strcmp("unset", cmd)))
		ft_putchar_fd('\'', STDERR_FILENO);
	if (more && *more)
	{
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(more, STDERR_FILENO);
	}
	ft_putchar_fd('\n', STDERR_FILENO);
	return (nb);
}

void	free_mini(t_mini *mini)
{
	free_history(&(mini->history));
	free_inputs(mini);
	ft_lstclear(&mini->env, free);
	if (mini->envp)
		ft_free_splited(mini->envp);
	if (mini->instructions)
		ft_instruclear(&mini->instructions);
	close(mini->stdin_copy);
	close(mini->stdout_copy);
	free(mini);
}

char	**get_env_tab_for_sort(t_list *env)
{
	char	**envp;
	int		i;
	int		size;

	size = ft_lstsize(env);
	if (!(envp = malloc(sizeof(char *) * (size + 1))))
		return (NULL);
	i = -1;
	while (++i < size)
	{
		envp[i] = ((char *)env->content);
		env = env->next;
	}
	envp[i] = NULL;
	return (envp);
}

int		run_builtins(char	**splited_inputs, t_mini *mini)
{
	if (!splited_inputs || !(*splited_inputs))
		return (0);
	if (ft_strcmp(splited_inputs[0], "echo") == 0)
		mini->last_return = echo_builtin(splited_inputs);
	else if (ft_strcmp(splited_inputs[0], "cd") == 0)
		mini->last_return = cd_builtin(mini, splited_inputs[1], splited_inputs);
	else if (ft_strcmp(splited_inputs[0], "pwd") == 0)
		mini->last_return = pwd_builtin();
	else if (ft_strcmp(splited_inputs[0], "export") == 0)
			mini->last_return = export_builtin(mini, &splited_inputs[1], mini->env);
	else if (ft_strcmp(splited_inputs[0], "unset") == 0)
			mini->last_return = unset_builtin(mini, &splited_inputs[1], mini->env);
	else if (ft_strcmp(splited_inputs[0], "exit") == 0)
		mini->last_return = exit_minishell(splited_inputs, mini);
	else if (ft_strcmp(splited_inputs[0], "env") == 0)
		mini->last_return = env_builtin(mini->env);
	else
		return (0);
	return (1);
}

int				ft_get_fd_token(const char *s)
{
	int	len;

	len = 0;
	if (s[len] == '<')
		len += 1;
	else if (s[len] == '>' && s[len + 1] != '>')
		len += 1;
	else
		len += 2;
	while (s[len] && (s[len] == ' ' || s[len] == 9))
		len++;
	len += ft_word_size(s + len);
	return (len);
}

int			quote_error_in_parsing(char c)
{
	ft_putstr_fd("\U0000274C minishell: quote ", STDERR_FILENO);
	ft_putchar_fd(c, STDERR_FILENO);
	ft_putstr_fd(" is not closed\n", STDERR_FILENO);
	return (-1);
}

int		ft_cmd_size(const char *s, char c)
{
	int		len;
	char	quote;

	len = 0;
	while (s[len])
	{
		if (s[len] == c)
			return (len);
		else if (s[len] == '\"' || s[len] == '\'')
		{
			quote = s[len];
			while (s[++len])
			{
				if (s[len] == '\\')
					len += 1;
				else if (s[len] == quote)
					break ;
			}
		}
		else if (s[len] == '\\')
			len += 1;
		if (!s[len])
			return (len);
		len++;
	}
	return (len);
}

int		ft_word_size(const char *s)
{
	int		len;

	len = 0;
	if ((s[len] == '>' || s[len] == '<'))
		return (ft_get_fd_token(s));
	while (s[len])
	{
		if (s[len] == ' ' || s[len] == 9 ||
				s[len] == '>' || s[len] == '<')
			return (len);
		else if (s[len] == '\"' || s[len] == '\'')
			jump_quotes_in_parsing((char *) s, s[len], &len);
		else if (s[len] == '\\')
			len += 1;
		if (s[len])
			len++;
	}
	return (len);
}

int		check_cmd_end(char c, int *isnt_blank, int *count)
{
	if (c != ' ' && c != 9 && c != ';')
			*isnt_blank = 1;
	if (c == ';')
	{
		if (!*isnt_blank)
			return (0);
		*count += 1;
		*isnt_blank = 0;
	}
	return (1);
}

int		jump_quotes_in_parsing(char *input, char quote, int *i)
{
	while (input[++(*i)])
	{
		if (input[*i] == '\\')
			*i += 1;
		else if (input[*i] == quote)
				break ;
	}
	return (1);
}

int		detect_quotes_in_input(char *input, char quote, int *i)
{
	jump_quotes_in_parsing(input, quote, i);
	if (!input[*i])
		return (quote_error_in_parsing(quote));
	return (1);
}

int		cmd_count(char *input)
{
	int		i;
	int		count;
	int		isnt_blank;

	isnt_blank = 0;
	i = 0;
	count = 1;
	while (input[i])
	{
		if (!check_cmd_end(input[i], &isnt_blank, &count))
			return (0);
		else if (input[i] == '\"' || input[i] == '\'')
		{
			if (detect_quotes_in_input(input, input[i], &i) == -1)
				return (-1);
		}
		else if (input[i] == '\\')
			i++;
		if (!input[i])
			return (count);
		i++;
	}
	return (count);
}

int		parsing_error(char c)
{
	ft_putstr_fd("\U0000274C minishell: ", STDERR_FILENO);
	ft_putstr_fd("syntax error near unexpected token", STDERR_FILENO);
	if (c == '|')
		ft_putstr_fd(" \'|\'\n", STDERR_FILENO);
	else if (c == '>')
		ft_putstr_fd(" \'newline\'\n", STDERR_FILENO);
	else if (c == ';')
		ft_putstr_fd(" \';\'\n", STDERR_FILENO);
	return (258);
}

t_list	*ft_lst_input(t_mini *mini, t_instructions *instruc, char *s)
{
	t_list	*cmd;
	int		len;

	if (!s)
		return (NULL);
	cmd = NULL;
	while (*s)
	{
		while ((*s == ' ' || *s == 9) && *s)
			s++;
		if (!(*s))
			return (cmd);
		len = ft_word_size(s);
		if (*s == '>')
		{
			instruc->is_empty = 0;
			if (get_fdout_file(instruc, ft_strndup(s, len), mini))
				return (cmd);
		}
		else if (*s == '<')
		{
			instruc->is_empty = 0;
			if (get_fdin_file(instruc, ft_strndup(s, len), mini))
				return (cmd);
		}
		else
			(ft_lstadd_back(&cmd, ft_lstnew(get_real_input(ft_strndup(s, len), mini, mini->env))));
		s += len;
	}
	if (instruc->is_empty == 0 && cmd == NULL)
		cmd = ft_lstnew(ft_strdup(""));
	return (cmd);
}

char		**get_cmd_tab(t_list *cmd)
{
	char	**cmd_tab;
	int		size;
	int		i;
	t_list	*elem;

	size = ft_lstsize(cmd);
	cmd_tab = malloc(sizeof(char *) * (size + 1));
	if (cmd_tab == NULL)
		return (NULL);
	elem = cmd;
	i = -1;
	while (++i > -1 && elem)
	{
		if (elem->content)
		{
			if (!(cmd_tab[i] = ft_strdup((char *)elem->content)))
				break;
		}
		else
			--i;
		elem = elem->next;
	}
	cmd_tab[i] = NULL;
	return (cmd_tab);
}

t_history	*ft_historynew(void)
{
	t_history	*elem;

	if (!(elem = malloc(sizeof(t_history) * 1)))
		return (NULL);
	elem->input = malloc(sizeof(char) *1);
	*elem->input = 0;
	elem->is_prompt = 1;
	elem->previous = NULL;
	elem->next = NULL;
	return (elem);
}

void	ft_history_add_front(t_history **ahist, t_history *new)
{
	if (!ahist)
		return ;
	(*ahist)->previous = new;
	new->next = *ahist;
	*ahist = new;
}

int		get_instructions(t_mini *mini, char *s)
{
	char			*instruction;
	t_instructions	*current;
	t_list			*cmd;
	int				len;

	mini->instructions = NULL;
	if (*s == '|')
		return (parsing_error('|'));
	while (*s)
	{
		if (*s == '|')
			s+= 1;
		current = ft_instructnew(NULL);
		len = ft_cmd_size(s, '|');
		if (s[len] == '|')
			mini->is_pipe = 1;
		if (!(instruction = ft_strndup(s, len)))
		{
			print_errors("malloc", strerror(errno), NULL, 1);
			return (1);
		}
		cmd = ft_lst_input(mini, current, instruction);
		current->cmds = cmd;
		ft_instruct_add_back(&mini->instructions, current);
		free(instruction);
		if (mini->is_pipe && current->is_empty && (!cmd || !*((char *)cmd->content)))
			return (parsing_error('|'));
		s += len;
	}
	return (0);
}

void		free_current_cmd(t_mini *mini, char *cmd_input)
{
	free_mini_tab(mini->envp);
	free_cmds(mini);
	mini->cmds = NULL;
	if (cmd_input)
		free(cmd_input);
	cmd_input = NULL;
	set_mini(mini);
}

t_list		*ft_lst_cmds(t_mini *mini, char *s)
{
	char	*cmd_input;
	int		cmd_nb;
	int		i;
	int		len;

	if (!s)
		return (NULL);
	cmd_nb = cmd_count(s);
	if (cmd_nb < 1)
	{
		if (!cmd_nb)
			mini->last_return = parsing_error(';');
		else
			mini->last_return = 258;
		free_current_cmd(mini, NULL);
		return (NULL);
	}
	i = 0;
	while (i < cmd_nb)
	{
		len = ft_cmd_size(s, ';');
		cmd_input = ft_strndup(s, len);
		mini->last_return = get_instructions(mini, cmd_input);
		free(cmd_input);
		cmd_input = NULL;
		if (mini->last_return)
		{
			free_current_cmd(mini, cmd_input);
			return (NULL);
		}
		if (mini->is_pipe)
			make_pipe(mini, mini->instructions);
		else
		{
			if (mini->instructions)
			{
				mini->cmd = get_cmd_tab(mini->instructions->cmds);
				run_cmd(mini, mini->cmd, mini->instructions);
			}
		}
		free_current_cmd(mini, cmd_input);
		s += len + 1;
		i++;
	}
	return (mini->cmds);
}

void redirect(int oldfd, int newfd) {
	if (oldfd != newfd)
		if (dup2(oldfd, newfd) != -1)
			close(oldfd);
}

char	*ft_strjoin_path(char const *s1, char const *s2)
{
	char	*str;
	int		i;

	if (s1 == NULL || !s2)
		return (NULL);
	if (!(str = (char *)malloc(sizeof(char) *
					(ft_strlen(s1) + ft_strlen(s2) + 2))))
		return (NULL);
	i = 0;
	while (*s1)
	{
		str[i] = *s1;
		s1++;
		i++;
	}
	str[i] = '/';
	i++;
	while (*s2)
	{
		str[i] = *s2;
		s2++;
		i++;
	}
	str[i] = '\0';
	return (str);
}

char	**transform_env_lst_in_tab(t_list *env)
{
	char	**envp;
	int		i;
	int		size;

	size = ft_lstsize(env);
	if (!(envp = malloc(sizeof(char *) * (size + 1))))
		return (NULL);
	i = 0;
	while (env && i < size)
	{
		if (ft_strchr((char *)env->content, '='))
		{
			envp[i] = ft_strdup((char *)env->content);
			if (!envp[i])
				break;
			i++;
		}
		env = env->next;
	}
	envp[i] = NULL;
	return (envp);
}

void	set_mini(t_mini *mini)
{
	mini->cmd = NULL;
	mini->envp = NULL;
	mini->cmds = NULL;
	mini->bin = NULL;
	mini->is_pipe = 0;
}

void	free_cmds(t_mini *mini)
{
	ft_lstclear(&mini->cmds, free);
	ft_instruclear(&mini->instructions);
	if (mini->bin)
		free(mini->bin);
	free_mini_cmd(mini);
	free(mini->cmds);
	mini->cmds = NULL;
	set_mini(mini);
}

int		ft_get_input(t_mini *mini)
{
	sig_catcher.should_run = 1;
	sig_catcher.pid = -1;
	if (!mini->last_return)
		ft_putstr_fd("\U0001F49A ", STDOUT_FILENO);
	else
		ft_putstr_fd("\U0001F494 ", STDOUT_FILENO);
	ft_putstr_fd("\033[0;34mminishell> \033[0m", STDOUT_FILENO);
	if (!(read_prompt(mini)))
	{
		print_errors("prompt", strerror(errno), NULL, 1);
		exit_minishell(NULL, mini);
	}
	if (sig_catcher.should_run == 0)
	{
		mini->last_return = 1;
		if (dup2(mini->stdin_copy, STDIN_FILENO) == -1)
		{
			print_errors("dup", strerror(errno), NULL, 1);
			exit_minishell(NULL, mini);
		}
		reset_input_mode();
		ft_bzero(mini->input, ft_strlen(mini->input));
		ft_bzero(mini->history->input, ft_strlen(mini->history->input));
		return(1);
	}
	if (ft_strchr(mini->input, '\n'))
		*(ft_strchr(mini->input, '\n')) = '\0';
	ft_lst_cmds(mini, mini->input);
	free(mini->input);
	mini->input = NULL;
	return (1);
}

t_mini	*init_mini(t_list *env)
{
	t_mini	*mini;

	if (!(mini = malloc(sizeof(t_mini))))
		return (NULL);
	mini->input = NULL;
	mini->env = env;
	mini->stdin_copy = dup(STDIN_FILENO);
	mini->stdout_copy = dup(STDOUT_FILENO);
	mini->instructions = NULL;
	mini->history = NULL;
	mini->last_return = 0;
	mini->cm_cap = tgetstr("cm", NULL);
	mini->dc_cap = tgetstr("dc", NULL);
	mini->le_cap = tgetstr("le", NULL);
	mini->ce_cap = tgetstr("ce", NULL);
	set_mini(mini);
	if (mini->stdin_copy == -1 || mini->stdout_copy == -1
			|| !mini->cm_cap || !mini->dc_cap || !mini->le_cap || !mini->ce_cap)
	{
		free(mini->env);
		if (mini->stdin_copy >= 0)
			close(mini->stdin_copy);
		if (mini->stdout_copy >= 0)
			close(mini->stdout_copy);
		free(mini);
		mini = NULL;
	}
	return (mini);
}

t_list		*set_basic_env(void)
{
	char	*value;
	char	*pwd;
	char	*envp[5];
	t_list	*env;

	value = getcwd(NULL, 0);
	if (!value)
		return (NULL);
	pwd = ft_strjoin("PWD=", value);
	if (!pwd)
		return (NULL);
	envp[0] = pwd;
	envp[1] = "SHLVL=0";
	envp[2] = "TERM=xterm-256color";
	envp[3] = "PATH=/bin";
	envp[4] = NULL;
	env = copy_env(envp);
	free(value);
	free(pwd);
	return (env);
}

int		main(int argc, char **argv, char **envp)
{
	t_mini *mini;
	int		ret;
	char	*term_type;
	t_list	*env;

	(void) argc;
	(void) argv;
	if (!envp || !*envp)
		env = set_basic_env();
	else
		env = copy_env(envp);
	if (!env)
		return (print_errors("malloc", strerror(errno), NULL, 1));
	term_type = get_env_var("TERM", env);
	ret = tgetent(NULL, term_type);
	if (!term_type || ret < 1)
	{
		free(env);
		free(term_type);
		return (print_errors("termcaps", "bad terminal type", NULL, 1));
	}
	free(term_type);
	if (!(mini = init_mini(env)))
		return (print_errors("init minishell", strerror(errno), NULL, 1));
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, sig_handler);
	while (1)
		ft_get_input(mini);
}
