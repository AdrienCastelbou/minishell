#include "minishell.h"

t_sigcatch	sig_catcher;

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

long long int	ft_atolli(const char *str)
{
	int			i;
	int			is_neg;
	long long int	result;

	i = 0;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	is_neg = 1;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			is_neg *= -1;
		i++;
	}
	result = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * is_neg);
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
	if (!ft_strcmp("export", cmd) || !ft_strcmp("unset", cmd))
		ft_putchar_fd('\'', STDERR_FILENO);
	if (cmd && *cmd)
		ft_putstr_fd(error, STDERR_FILENO);
	if (!ft_strcmp("export", cmd) || !ft_strcmp("unset", cmd))
		ft_putchar_fd('\'', STDERR_FILENO);
	if (more && *more)
	{
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(more, STDERR_FILENO);
	}
	ft_putchar_fd('\n', STDERR_FILENO);
	return (nb);
}

int		is_only_digit(char *s)
{
	int i;

	i = 0;
	if (!ft_isdigit(s[i]) && s[i] != '-' && s[i] != '+')
		return (0);
	while (s[++i])
		if (!ft_isdigit(s[i]))
			return (0);
	return (1);
}

int		is_bad_num_value(char *s, long long int nb)
{
	if (nb > 0 && (*s == '-'))
		return (1);
	else if (nb < 0 && (*s != '-'))
		return (1);
	return (0);
}

int		exit_minishell(char	**splited_inputs, t_mini *mini)
{
	long long int	return_value;

	ft_putstr_fd("exit\n", STDERR_FILENO);
	return_value = mini->last_return;
	if (splited_inputs)
	{
		if (splited_inputs[1] && !is_only_digit(splited_inputs[1]))
			return_value = print_errors("exit", splited_inputs[1], "numeric argument required", 255);
		else if (splited_inputs[1] && splited_inputs[2])
			return (print_errors("exit", "too many arguments", NULL, 1));
		else if (splited_inputs[1])
			return_value = ft_atolli(splited_inputs[1]);
		else if (is_bad_num_value(splited_inputs[1], return_value))
			return_value = print_errors("exit", splited_inputs[1], "numeric argument required", 255);
	}
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
	exit(return_value);
	return (0);
}

int		echo_builtin(char	**splited_inputs)
{
	int	i;
	int	is_flag;

	i = 0;
	is_flag = 0;
	if (splited_inputs[1] && ft_strcmp(splited_inputs[1], "-n") == 0)
		is_flag = 1;
	i += is_flag;
	while (splited_inputs[++i])
	{
		ft_putstr_fd(splited_inputs[i], 1);
		if (splited_inputs[i + 1])
			write(1, " ", 1);
	}
	if (!is_flag)
		write(1, "\n", 1);
	return (0);
}

int		env_builtin(t_list *env)
{
	while (env)
	{
		if (ft_strchr((char *)env->content, '='))
		{
			ft_putstr_fd((char *)(env->content), STDOUT_FILENO);
			ft_putchar_fd('\n', STDOUT_FILENO);
		}
		env = env->next;
	}
	return (0);
}

void	add_env_var(char *env_var, t_list *env, char *key)
{
	t_list	*elem;
	t_list	*begin;
	int		key_len;

	key_len = ft_strlen(key);
	begin = env;
	while (begin)
	{
		if (ft_strnstr((char *)begin->content, key, key_len) == begin->content
				&& (*(char *)(begin->content + key_len) == '=' || !*(char *)(begin->content + key_len)))
		{
			free(key);
			free(begin->content);
			begin->content = ft_strdup(env_var);
			return ;
		}
		begin = begin->next;
	}
	elem = ft_lstnew(ft_strdup(env_var));
	ft_lstadd_back(&env, elem);
	free(key);
}

void	ft_switch_strs(char **tab_var, int i, int j)
{
	char	*tmp;

	tmp = tab_var[i];
	tab_var[i] = tab_var[j];
	tab_var[j] = tmp;
}

void	print_ordered_var(char *str)
{
	int		key_len;
	char	*key;

	key_len = 0;
	while (str[key_len] && str[key_len] != '=')
		key_len++;
	key = ft_strndup(str, key_len);
	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	ft_putstr_fd(key, STDOUT_FILENO);
	if (str[key_len] == '=')
	{
		ft_putstr_fd("=\"", STDOUT_FILENO);
		ft_putstr_fd(str + key_len + 1, STDOUT_FILENO);
		ft_putstr_fd("\"\n", STDOUT_FILENO);
	}
	else
		ft_putchar_fd('\n', STDOUT_FILENO);
	free(key);
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

void	print_export_var(t_list *env)
{
	char	**tab_var;
	int		i;
	int		j;

	tab_var = get_env_tab_for_sort(env);
	i = -1;
	while (tab_var[++i])
	{
		j = i;
		while (tab_var[++j])
			if (ft_strcmp(tab_var[i], tab_var[j]) > 0)
				ft_switch_strs(tab_var, i, j);
	}
	i = -1;
	while (tab_var[++i])
		print_ordered_var(tab_var[i]);
	i = -1;
	free(tab_var);
}

int		is_valid_env_char(char c, int i)
{
	if (i == 0 && (ft_isdigit(c) || c == 0))
		return (0);
	else if (ft_isalnum(c) || c == '_')
		return (1);
	return (0);
}
int		export_builtin(t_mini *mini, char	**splited_inputs, t_list *env)
{
	int	i;
	int	j;

	i = -1;
	if (!*splited_inputs)
	{
		print_export_var(env);
		return (0);
	}
	while (splited_inputs[++i])
	{
		mini->last_return = 0;
		j = -1;
		while (splited_inputs[i][++j] && is_valid_env_char(splited_inputs[i][j], j))
			;
		if (j > 0 && (splited_inputs[i][j] == '=' || (!splited_inputs[i][j])))
			add_env_var(splited_inputs[i], env, ft_strndup(splited_inputs[i], j));
		else
			mini->last_return = print_errors("export", splited_inputs[i], "not a valid identifier", 1);
	}
		return (mini->last_return);
}

void	delete_env_var_elem(t_list **env, t_list **current, t_list **previous)
{
	if (!previous)
		*env = (*current)->next;
	else
		(*previous)->next = (*current)->next;
	ft_lstdelone(*current, free);
	if (previous)
		*current = (*previous)->next;
	else
		*current = *env;
}

void	delete_env_var(char *key, t_list **env)
{
	t_list	*previous;
	t_list	*current;
	int		key_len;

	key_len = ft_strlen(key);
	previous = NULL;
	current = *env;
	while (current)
	{
		if (ft_strnstr((char *)current->content, key, key_len) == current->content
				&& (*(char *)(current->content + key_len) == '=' || !*(char *)(current->content + key_len)))
			delete_env_var_elem(env, &current, &previous);
		else
		{
			previous = current;
			current = previous->next;
		}
	}
}

int		unset_builtin(t_mini *mini, char	**splited_inputs, t_list *env)
{
	int	i;
	int	j;

	i = -1;
	while (splited_inputs[++i])
	{
		mini->last_return = 0;
		j = -1;
		while (splited_inputs[i][++j] && is_valid_env_char(splited_inputs[i][j], j))
			;
		if (!splited_inputs[i][j] && j > 0)
			delete_env_var(splited_inputs[i], &env);
		else
			mini->last_return = print_errors("export", splited_inputs[i], "not a valid identifier", 1);
	}
		return (mini->last_return);
}

int		pwd_builtin(void)
{
	char	buff[128];
	char	*buff_copy;
	int		status;

	status = 0;
	ft_bzero(buff, 128);
	if ((buff_copy = getcwd(buff, 128)) != NULL)
	{
		ft_putstr_fd(buff_copy, STDOUT_FILENO);
		ft_putchar_fd('\n', STDOUT_FILENO);
	}
	else
		status = 1;
	return (status);
}

char	*get_user_dir(t_mini *mini)
{
	char	*user;
	char	*user_path;

	user = get_env_var("USER", mini->env);
	user_path = ft_strjoin_path("/home", user);
	free(user);
	return (user_path);
}

void	update_pwd_paths(t_mini *mini)
{
	char	buff[128];
	char	*pwd[2];
	char	*old_path;

	old_path = get_env_var("PWD", mini->env);
	pwd[0] = ft_strjoin("OLDPWD=", old_path);
	pwd[1] = NULL;
	export_builtin(mini, pwd, mini->env);
	free(old_path);
	free(pwd[0]);
	ft_bzero(buff, 128);
	getcwd(buff, 128);
	pwd[0] = ft_strjoin("PWD=", buff);
	pwd[1] = NULL;
	export_builtin(mini, pwd, mini->env);
	free(pwd[0]);
}

int		cd_oldpwd(t_mini *mini)
{
	char *mov;

	mov = get_env_var("OLDPWD", mini->env);
	if (!mov || !*mov)
	{
		if (mov)
			free(mov);
		print_errors("cd", "OLDPWD", "not set", 1);
		return (1);
	}
	mini->last_return = chdir(mov);
	update_pwd_paths(mini);
	free(mov);
	return (0);
}

int		cwd_exist(void)
{
	char	buff[128];

	if (getcwd(buff, 128) == NULL)
		return (0);
	return (1);
}


int		cd_builtin(t_mini *mini, char *mov, char **inputs)
{
	int		mov_usr;

	if (cwd_exist() == 0)
	{
		print_errors("error", "getcwd cant access to parents directories", NULL, 0);
		mini->last_return = 0;
		return (mini->last_return);
	}

	if (mov && inputs[2])
	{
		print_errors("cd", "to many arguments", NULL, 1);
		mini->last_return = 1;
		return (mini->last_return);
	}
	mov_usr = 0;
	if (mov == NULL)
	{
		mov = get_user_dir(mini);
		mov_usr = 1;
	}
	else if (ft_strcmp(mov, "-") == 0)
		return (cd_oldpwd(mini));
	mini->last_return = chdir(mov);
	if (!mini->last_return)
		update_pwd_paths(mini);
	else if (mini->last_return == -1)
	{
		print_errors("cd", mov, strerror(errno), 1);
		mini->last_return = 1;
	}
	if (mov_usr)
		free(mov);
	return (mini->last_return);
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

static int		ft_cmd_size(const char *s, char c)
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

static int		ft_word_size(const char *s)
{
	int		len;
	char	quote;

	len = 0;
	if ((s[len] == '>' || s[len] == '<'))
		return (ft_get_fd_token(s));
	while (s[len])
	{
		if (s[len] == ' ' || s[len] == 9)
			return (len);
		else if (s[len] == '>' || s[len] == '<')
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

char			*get_env_var(const char *s, t_list *env)
{
	int		i;
	char	*variable;
	char	*str;

	str = NULL;
	i = -1;
	while (s[++i] && ft_isenvchar(s[i]))
		;
	if (!(variable = ft_strndup((char *)s, i)))
		return (NULL);
	while (env)
	{
		if (ft_strnstr((char *)env->content, variable, i) == env->content
				&& *(char *)(env->content + i) == '=')
		{
			str = ft_strdup(env->content + i + 1);
			break;
		}
		env = env->next;
	}
	free(variable);
	return (str);
}

char			*join_input_parts(char *s, char *new, int i)
{
	char	*dup_s;
	char	*tmp;

	dup_s = ft_strndup(s, i);
	tmp = new;
	new = ft_strjoin(tmp, dup_s);
	free(dup_s);
	free(tmp);
	return (new);
}

char			*get_var_value(char *s, char *new, int i, t_list *env)
{
	char	*var;
	char	*tmp;

	new = join_input_parts(s, new, i);
	var = get_env_var(&s[i + 1], env);
	tmp = new;
	new = ft_strjoin(tmp, var);
	free(tmp);
	free(var);
	return (new);
}

char			*update_input_with_var(char **s, char *new, int *i, t_list *env)
{
	char	*str;

	str = *s;
	new = get_var_value(*s, new, *i, env);
	if ((!new || *new != 0) || ft_isalpha(str[*i + 1]))
		while (str[++(*i)] && ft_isenvchar(str[*i]))
			;
	else
		*i += 2;
	*s = *s + *i;
	*i = 0;
	return (new);
}

int				ft_isenvchar(char c)
{
	if (ft_isalnum(c) || c == '_')
		return (1);
	return (0);
}

char			*update_input_with_big_quotes(char **s, char *new, int *i, t_mini *mini)
{
	char *str;

	new = join_input_parts(*s, new, *i);
	*s = *s + *i + 1;
	str = *s;
	*i = 0;
	while (str[*i] != '\"' && str[*i])
	{
		if (str[*i] == '\\' && (str[*i + 1] == '$' || str[*i + 1] == '\"' || str[*i + 1] == '\\'))
			new = update_input_with_echap(&str, new, i);
		else if (str[*i] == '$' && str[*i + 1] && ft_isenvchar(str[*i + 1]))
			new = update_input_with_var(&str, new, i, mini->env);
		else if (str[*i] == '$' && str[*i + 1] == '?')
			new = update_input_with_last_return(&str, new, i, mini);
		else
			*i += 1;
	}
	*s = str;
	new = join_input_parts(*s, new, *i);
	*s = *s + *i;
	if (**s)
		*s += 1;
	*i = 0;
	return (new);
}

char			*update_input_with_lil_quotes(char **s, char *new, int *i)
{
	char	*str;

	new = join_input_parts(*s, new, *i);
	*s = *s + *i + 1;
	str = *s;
	*i = 0;
	while (str[*i] != '\'' && str[*i])
		*i +=1;
	new = join_input_parts(*s, new, *i);
	*s = *s + *i;
	if (**s)
		*s += 1;
	*i = 0;
	return (new);
}

char			*update_input_with_echap(char **s, char *new, int *i)
{
	char	buff[2];
	char	*str;

	str = *s;
	*buff = str[*i + 1];
	buff[1] = 0;
	new = join_input_parts(*s, new, *i);
	new = join_input_parts(buff, new, 1);
	*s = *s + *i;
	if (**s)
		*s += 1;
	if (**s)
		*s += 1;
	*i = 0;
	return (new);
}

char			*update_input_with_last_return(char **s, char *new, int *i, t_mini *mini)
{
	char	*last_return;
	char	*tmp;

	new = join_input_parts(*s, new, *i);
	last_return = ft_itoa(mini->last_return);
	tmp = new;
	new = ft_strjoin(tmp, last_return);
	free(tmp);
	free(last_return);
	*s = *s + *i + 2;
	*i = 0;
	return (new);
}

char			*get_real_input(char *s, t_mini *mini, t_list *env)
{
	int		i;
	char	*new;
	char	*str;

	str = s;
	new = malloc(sizeof(char) * 1);
	*new = 0;
	i = 0;
	while (s[i] && s[i] != ' ' && s[i] != 9)
	{
		if (s[i] == '\\')
			new = update_input_with_echap(&s, new, &i);
		else if (s[i] == '$' && s[i + 1] && ft_isenvchar(s[i + 1]))
			new = update_input_with_var(&s, new, &i, env);
		else if (s[i] == '\"')
			new = update_input_with_big_quotes(&s, new, &i, mini);
		else if (s[i] == '\'')
			new = update_input_with_lil_quotes(&s, new, &i);
		else if (s[i] == '$' && s[i + 1] == '?')
			new = update_input_with_last_return(&s, new, &i, mini);
		else
			i++;
	}
	new = join_input_parts(s, new, i);
	free(str);
	return (new);
}

int		cmd_count(char *input)
{
	int		i;
	int		count;
	int		isnt_blank;
	char	quote;

	isnt_blank = 0;
	i = 0;
	count = 1;
	while (input[i])
	{
		if (input[i] != ' ' && input[i] != 9 && input[i] != ';')
			isnt_blank = 1;
		if (input[i] == ';')
		{
			if (!isnt_blank)
				return (0);
			count += 1;
			isnt_blank = 0;
		}
		else if (input[i] == '\"' || input[i] == '\'')
		{
			quote = input[i];
			while (input[++i])
			{
				if (input[i] == '\\')
					i += 1;
				else if (input[i] == quote)
					break ;
			}
			if (!input[i])
				return (quote_error_in_parsing(quote));
		}
		else if (input[i] == '\\')
			i++;
		if (!input[i])
			return (count);
		i++;
	}
	return (count);
}


void	ft_fdsadd_back(t_fds **afds, t_fds *new)
{
	t_fds *elem;

	if (!afds)
		return ;
	if (!*afds)
	{
		*afds = new;
		return ;
	}
	elem = *afds;
	while (elem->next)
		elem = elem->next;
	elem->next = new;
}

t_fds	*ft_fdnew(char *file, char *method)
{
	t_fds	*elem;

	if (!(elem = (t_fds *)malloc(sizeof(t_fds))))
		return (NULL);
	if (strcmp(method, ">") == 0)
		elem->fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
	else
		elem->fd = open(file, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
	elem->next = NULL;
	return (elem);
}

t_fds	*ft_fdlast(t_fds *fd)
{
	if (!fd)
		return (NULL);
	while (fd->next)
		fd = fd->next;
	return (fd);
}

int		open_agreg_file(char *file, char *method)
{
	int fd;

	if (strcmp(method, ">") == 0)
		fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
	else if (strcmp(method, "<") == 0)
		fd = open(file, O_RDONLY);
	else
		fd = open(file, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
	return (fd);
}
int		create_and_close_file(char *file, char *method)
{
	int fd;

	if (strcmp(method, ">") == 0)
		fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
	else if (strcmp(method, "<") == 0)
		fd = open(file, O_RDONLY);
	else
		fd = open(file, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
	if (fd < 0)
		return (print_errors(file, strerror(errno), NULL, 1));
	close(fd);
	return (0);
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

int		get_fdout_file(t_instructions *instruct, char *s, t_mini *mini)
{
	char	*method;
	char	*file;
	int		i;
	int		size;

	i = 0;
	if (s[i] == '>' && s[i + 1] != '>')
	{
		i += 1;
		method = ">";
	}
	else if (s[i] == '>' && s[i + 1] == '>')
	{
		i += 2;
		method = ">>";
	}
	while (s[i] && (s[i] == ' ' || s[i] == 9))
		i++;
	if (s[i] == 0)
	{
		free(s);
		return (parsing_error('>'));
	}
	size = ft_word_size(s + i);
	if (instruct->fdout.name)
	{
		free(instruct->fdout.name);
		instruct->fdout.name = NULL;
	}
	file = get_real_input(ft_strndup(s + i, size), mini, mini->env);
	instruct->fdout.name = file;
	instruct->fdout.method = method;
	instruct->fdout.is_file = 1;
	create_and_close_file(file, method);
	free(s);
	return (0);
}

int		get_fdin_file(t_instructions *instruct, char *s, t_mini *mini)
{
	char	*method;
	char	*file;
	int		size;
	int		i;

	method = "<";
	i = 1;
	while (s[i] && (s[i] == ' ' || s[i] == 9))
		i++;
	if (s[i] == 0)
	{
		free(s);
		return (parsing_error('>'));
	}
	size = ft_word_size(s + i);
	if (instruct->fdin.name)
	{
		free(instruct->fdin.name);
		instruct->fdin.name = NULL;
	}
	file = get_real_input(ft_strndup(s + i, size), mini, mini->env);
	instruct->fdin.name = file;
	instruct->fdin.method = method;
	instruct->fdin.is_file = 1;
	free(s);
	return (create_and_close_file(file, method));
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
	elem = cmd;
	i = -1;
	while (++i > -1 && elem)
	{
		if (elem->content)
			cmd_tab[i] = ft_strdup((char *)elem->content);
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

void	ft_instruct_add_back(t_instructions **alst, t_instructions *new)
{
	t_instructions *elem;

	if (!alst)
		return ;
	if (!*alst)
	{
		*alst = new;
		return ;
	}
	elem = *alst;
	while (elem->next)
		elem = elem->next;
	elem->next = new;
}

t_instructions	*ft_instructnew(t_list *content)
{
	t_instructions	*elem;

	if (!(elem = (t_instructions*)malloc(sizeof(t_instructions))))
		return (NULL);
	elem->cmds = content;
	elem->fdin.fd = 0;
	elem->fdin.name = NULL;
	elem->fdin.is_file = 0;
	elem->is_empty = 1;
	elem->fdout.fd = 1;
	elem->fdout.is_file = 0;
	elem->fdout.name = NULL;
	elem->next = NULL;
	return (elem);
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
		instruction = ft_strndup(s, len);
		cmd = ft_lst_input(mini, current, instruction);
		if (mini->is_pipe && current->is_empty && (!cmd || !*((char *)cmd->content)))
			return (parsing_error('|'));
		current->cmds = cmd;
		ft_instruct_add_back(&mini->instructions, current);
		free(instruction);
		s += len;
	}
	return (0);
}

void		make_pipe(t_mini *mini, t_instructions *instruc);

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

void	exec_cmd(t_mini *mini, char **cmd)
{
	char	*path_list;

	mini->envp = transform_env_lst_in_tab(mini->env);
	if (!cmd || !*cmd || !**cmd)
		;
	else if (run_builtins(cmd, mini))
		;
	else
	{
		path_list = get_env_var("PATH", mini->env);
		sig_catcher.pid = fork();
		if (sig_catcher.pid == 0)
		{
			if (!ft_strchr(cmd[0], '/'))
				run_bin(cmd, mini, path_list);
			else
				execve(cmd[0], cmd, mini->envp);
			print_errors(cmd[0], strerror(errno), NULL, 127);
			exit(127);
		}
		else
		{
			if (0 < waitpid(sig_catcher.pid, &(mini->last_return), 0) && WIFEXITED(mini->last_return))
				mini->last_return = WEXITSTATUS(mini->last_return);
			if ((mini->last_return == 2 || mini->last_return == 3) && sig_catcher.should_run == 0)
				mini->last_return += 128;
			if (path_list)
				free(path_list);
		}
	}

}

static void redirect(int oldfd, int newfd) {
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

int		run_bin(char **cmd, t_mini *mini, char *path_list)
{
	char	*path;
	int		path_len;

	mini->bin = ft_strdup(cmd[0]);
	while (path_list && *path_list)
	{
		path_len = 0;
		while (path_list[path_len] && path_list[path_len] != ':')
			path_len++;
		path = ft_strndup(path_list, path_len);
		free(cmd[0]);
		cmd[0] = ft_strjoin_path(path, mini->bin);
		free(path);
		execve(cmd[0], cmd, mini->envp);
		path_list += path_len;
		if (*path_list == ':')
			path_list += 1;
	}
	ft_putstr_fd("\U0000274C minishell: ", STDERR_FILENO);
	ft_putstr_fd(mini->bin, STDERR_FILENO);
	ft_putstr_fd(": command not found\n", STDERR_FILENO);
	exit(127);
	return (0);
}

int		run(t_mini *mini, int fdin, int fdout)
{
	char	**cmd;
	char	*path_list;

	cmd = mini->cmd;
	mini->envp = transform_env_lst_in_tab(mini->env);
	path_list = get_env_var("PATH", mini->env);
	if (fdin == -1)
		exit(1);
	redirect(fdin, STDIN_FILENO);
	redirect(fdout, STDOUT_FILENO);
	if (!*cmd || !**cmd)
		;
	else if (run_builtins(cmd, mini))
		;
	else
	{
		if (!ft_strchr(cmd[0], '/'))
			run_bin(cmd, mini, path_list);
		else
			execve(cmd[0], cmd, mini->envp);
		print_errors(cmd[0], strerror(errno), NULL, 127);
		exit(127);
	}
	exit(mini->last_return);
	return (1);
}

void	pipe_loop(t_mini *mini, t_instructions *instruc, int fdin);

void	run_piped_child(t_mini *mini,  t_instructions *instruc, int fdin, int *pfd)
{
	close(pfd[0]);
	mini->cmd = get_cmd_tab(instruc->cmds);
	run(mini, fdin, pfd[1]);
	exit(127);
}

void	run_piped_parent(t_mini *mini, t_instructions *instruc, t_files_portal	fds, int pid)
{
	int	status;

	close(fds.pfd[1]);
	close(fds.fdin);
	fds.fdin = fds.pfd[0];
	if (instruc)
		pipe_loop(mini, instruc->next, fds.fdin);
	if (0 < waitpid(pid, &status, 0) && WIFEXITED(status))
	{
		if (!instruc->next)
		{
			mini->last_return = WEXITSTATUS(status);
			if ((mini->last_return == 2 || mini->last_return == 3) && sig_catcher.should_run == 0)
				mini->last_return += 128;
		}
	}
	close(fds.pfd[0]);
	sig_catcher.should_run = 1;
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	dup2(mini->stdin_copy, STDIN_FILENO);
	dup2(mini->stdout_copy, STDOUT_FILENO);
}

void	pipe_loop(t_mini *mini, t_instructions *instruc, int fdin)
{
	t_files_portal	fds;
	int	pid;

	fds.fdin = fdin;
	if (!instruc)
		return ;
	if (pipe(fds.pfd) == -1)
		return ;
	if (instruc->fdin.name)
	{
		if (fds.fdin != STDIN_FILENO)
			close(fds.fdin);
		fds.fdin = open_agreg_file(instruc->fdin.name, instruc->fdin.method);
	}
	if (instruc->fdout.name)
	{
		fds.fdout = open_agreg_file(instruc->fdout.name, instruc->fdout.method);
		dup2(fds.fdout, fds.pfd[1]);
		close(fds.fdout);
	}
	else if (!instruc->next)
	{
		close(fds.pfd[1]);
		fds.pfd[1] = STDOUT_FILENO;
	}
	if ((pid = fork()) < 0)
		return ;
	sig_catcher.pid = pid;
	if (pid == 0)
		run_piped_child(mini, instruc, fds.fdin, fds.pfd);
	else
		run_piped_parent(mini, instruc, fds, pid);
}

void	make_pipe(t_mini *mini, t_instructions *instruc)
{
	pipe_loop(mini, instruc, STDIN_FILENO);
}

void	run_cmd(t_mini *mini, char **cmd, t_instructions *instruc)
{
	int	fdin;
	int	fdout;

	fdin = STDIN_FILENO;
	fdout = STDOUT_FILENO;
	if (instruc->fdin.name) 
		fdin = open_agreg_file(instruc->fdin.name, instruc->fdin.method);
	if (fdin == -1)
	{
		mini->last_return = 1;
		return ;
	}
	if (instruc->fdout.name)
		fdout = open_agreg_file(instruc->fdout.name, instruc->fdout.method);
	redirect(fdin, STDIN_FILENO);
	redirect(fdout, STDOUT_FILENO);
	exec_cmd(mini, cmd);
	dup2(mini->stdin_copy, STDIN_FILENO);
	dup2(mini->stdout_copy, STDOUT_FILENO);
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

void	ft_fddelone(t_fds *fd)
{
	if (!fd)
		return ;
	if (fd)
		close(fd->fd);
	free(fd);
}

void	ft_fdclear(t_fds **fds)
{
	if (!fds || !*fds)
		return ;
	if ((*fds)->next != NULL)
		ft_fdclear(&((*fds)->next));
	ft_fddelone((*fds));
	*fds = NULL;
}

void	ft_instrucdelone(t_instructions *instruc)
{
	if (!instruc)
		return ;
	ft_lstclear(&(instruc->cmds), free);
	if (instruc->fdin.name)
		free(instruc->fdin.name);
	instruc->fdin.name = NULL;
	if (instruc->fdout.name)
		free(instruc->fdout.name);
	instruc->fdout.name = NULL;

	free(instruc);
}

void	ft_instruclear(t_instructions **instruc)
{
	if (!instruc || !*instruc)
		return ;
	if ((*instruc)->next != NULL)
		ft_instruclear(&((*instruc)->next));
	ft_instrucdelone((*instruc));
	*instruc = NULL;
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

void	set_mode(void)
{
	struct termios	t;
	int				r;

	tcgetattr (STDIN_FILENO, &sig_catcher.saved_attributes);
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

void	reset_input_mode (void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &sig_catcher.saved_attributes);
}

void	erase_char_in_prompt(t_mini *mini, int *top, char *buff)
{
	ioctl(STDIN_FILENO, TIOCGWINSZ, &mini->ws);
	if (!(*mini->history->input) && !*buff)
		return ;
	if (!*top && *mini->history->input)
		mini->history->input[ft_strlen(mini->history->input) - 1] = 0;
	else if (*top)
	{
		*top -= 1;
		buff[*top] = 0;
	}
	get_cursor_position(&(mini->cursor.cur_col), &(mini->cursor.cur_line));
	if (mini->cursor.cur_col > 1)
		tputs(mini->le_cap, 1, ft_putchar);
	else
		tputs(tgoto(mini->cm_cap, mini->ws.ws_col - 1, mini->cursor.cur_line - 2), 1, ft_putchar);
	tputs(mini->dc_cap, 1, ft_putchar);
}

int		join_prompt_parts(t_mini *mini, char *buff)
{
	char *tmp;

	tmp = mini->history->input;
	mini->history->input = ft_strjoin(tmp, buff);
	free(tmp);
	ft_bzero(buff, ft_strlen(buff));
	if (!mini->history->input)
		return (0);
	return (1);
}

void	write_char_in_prompt(t_mini *mini, char c, int *top, char *buff)
{
	buff[*top] = c;
	write(1, &c, 1);
	*top += 1;
	if (*top >= 128 - 1)
	{
		join_prompt_parts(mini, buff);
		*top = 0;
	}
}

int		is_arrow(char *buff)
{
	if (buff[0] == 27 && buff[1] == 91 && buff[2] == 65)
		return (1);
	else if (buff[0] == 27 && buff[1] == 91 && buff[2] == 66)
		return (1);
	return (0);
}

int		ft_putchar(int c)
{
	write(1, &c, 1);
	return (1);
}

void	erase_current_prompt(t_mini *mini, int *top, char *buff, t_cursor *cursor)
{
	int		len;
	int		i;
	int		isnt_endline;

	ioctl(STDIN_FILENO, TIOCGWINSZ, &mini->ws);
	isnt_endline = 1;
	get_cursor_position(&cursor->cur_col, &cursor->cur_line);
	len = ft_strlen(mini->history->input);
	i = -1;
	while (++i < len)
	{
		if (cursor->cur_col == mini->ws.ws_col)
			isnt_endline = 0;
		if (cursor->cur_col > 0)
			cursor->cur_col -= 1;
		else
		{
			tputs(tgoto(mini->cm_cap, 0, cursor->cur_line - 1), 1, ft_putchar);
			tputs(mini->ce_cap, 1, ft_putchar);
			cursor->cur_col = mini->ws.ws_col - 1;
			cursor->cur_line -= 1;
			tputs(tgoto(mini->cm_cap, cursor->cur_col, cursor->cur_line - 1), 1, ft_putchar);
		}
	}
	tputs(tgoto(mini->cm_cap, cursor->cur_col - isnt_endline, cursor->cur_line - 1), 1, ft_putchar);
	tputs(mini->ce_cap, 1, ft_putchar);
	ft_bzero(mini->history->input, ft_strlen(mini->history->input));
	ft_bzero(buff, ft_strlen(buff));
	*top = 0;
}

void	get_cursor_position(int *col, int *line)
{
	char cmd[]="\033[6n";
	char buff[128];
	int len = ft_strlen(cmd);
	ft_bzero(buff, 128);
	write(1, cmd, len);
	read(1, buff, 127);
	int i = 2;
	*line = ft_atoi(&buff[i]);
	while (buff[i] && buff[i] != ';')
		i++;
	i+= 1;
	*col = ft_atoi(&buff[i]);
}

void		add_input_in_history(t_mini *mini)
{
	t_history	*elem;

	if (mini->history && !*mini->history->input)
	{
		mini->current_hist = mini->history;
		return ;
	}
	elem = ft_historynew();
	if (mini->history)
		ft_history_add_front(&mini->history, elem);
	else
		mini->history = elem;
	mini->current_hist = mini->history;
}

void	up_history(t_mini *mini, int *top, char *buff, t_cursor *cursor)
{
	int		len;
	char	*new;

	if (!mini->current_hist->next)
		return ;
	join_prompt_parts(mini, buff);
	erase_current_prompt(mini, top, buff, cursor);
	mini->current_hist = mini->current_hist->next;
	new = mini->current_hist->input;
	len = ft_strlen(new);
	int i = -1;
	while (++i < len)
		write_char_in_prompt(mini, new[i], top, buff);
}

void	down_history(t_mini *mini, int *top, char * buff, t_cursor *cursor)
{
	int		len;
	char	*new;

	if (!mini->current_hist->previous)
		return ;
	join_prompt_parts(mini, buff);
	erase_current_prompt(mini, top, buff, cursor);
	mini->current_hist = mini->current_hist->previous;
	new = mini->current_hist->input;
	len = ft_strlen(new);
	int i = -1;
	while (++i < len)
		write_char_in_prompt(mini, new[i], top, buff);
}

void	check_input_validity(t_mini *mini, char *buffchar, int *top, char *buff)
{
	int	i;

	i = -1;
	while (++i < 3 && buffchar[i])
	{
		if (ft_isprint(buffchar[i]))
			write_char_in_prompt(mini, buffchar[i], top, buff);
	}
}

int		check_prompt_input(t_mini *mini, int *top, char *buffchar, char *buff)
{
	if (*buffchar == '\004')
	{
		if (!(*mini->history->input) && !*buff)
			exit_minishell(NULL, mini);
	}
	else if (is_arrow(buffchar) && buffchar[2] == 65)
		up_history(mini, top, buff, &mini->cursor);
	else if (is_arrow(buffchar) && buffchar[2] == 66)
		down_history(mini, top, buff, &mini->cursor);
	else if (*buffchar == '\n')
		return (0);
	else if (*buffchar == 127)
		erase_char_in_prompt(mini, top, buff);
	else if (ft_isprint(*buffchar))
		check_input_validity(mini, buffchar, top, buff);
	ft_bzero(buffchar, 3);
	return (1);
}

int		read_prompt(t_mini *mini)
{
	char		buffchar[3];
	char		buff[128];
	int			top;

	add_input_in_history(mini);
	mini->current_hist = mini->history;
	top = 0;
	ft_bzero(buff, 128);
	set_mode();
	ft_bzero(buffchar, 3);
	while (read(STDIN_FILENO, buffchar, 3) && sig_catcher.should_run)
		if (!check_prompt_input(mini, &top, buffchar, buff))
			break ;
	write(1, "\n", 1);
	reset_input_mode();
	if (!(join_prompt_parts(mini, buff)))
		return (0);
	if (!(mini->input = ft_strdup(mini->history->input)))
		return (0);
	mini->history->is_prompt = 0;
	return (1);
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

char	*get_shlvl(char *shlvl)
{
	char	*value;
	int		inted_value;
	char	*env_var;

	while (!ft_isdigit(*shlvl))
		shlvl++;
	inted_value = ft_atoi(shlvl) + 1;
	if (!(value = ft_itoa(inted_value)))
		return (NULL);
	env_var = ft_strjoin("SHLVL=", value);
	free(value);
	return (env_var);
}

t_list	*copy_env(char **envp)
{
	t_list	*env;
	t_list	*elem;
	char	*env_var;

	env = NULL;
	elem = NULL;
	while (*envp)
	{
		if (ft_strnstr(*envp, "SHLVL=", 6) == *envp)
			env_var = get_shlvl(*envp);
		else
			env_var = ft_strdup(*envp);
		if (!env_var)
		{
			ft_lstclear(&env, free);
			return (NULL);
		}
		if (!(elem = ft_lstnew(env_var)))
		{
			free(env_var);
			ft_lstclear(&env, free);
			return (NULL);
		}
		ft_lstadd_back(&env, elem);
		envp++;
	}
	return (env);
}

void sig_handler(int signum)
{
	if (signum == SIGINT)
	{
		sig_catcher.should_run = 0;
		close(STDIN_FILENO);
	}
	else if (signum == SIGQUIT && sig_catcher.pid > -1)
	{
		sig_catcher.should_run = 0;
		ft_putstr_fd("Quit\n", STDERR_FILENO);
		kill(sig_catcher.pid, SIGQUIT);
	}
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
	return (NULL);
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
