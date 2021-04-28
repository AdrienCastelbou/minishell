#include "minishell.h"
#include <termios.h>

t_sigcatch	sig_catcher;

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

void	free_inputs(t_mini *mini)
{
	int i;

	i = -1;
	if (mini->cmd)
	{
		while (mini->cmd[++i])
			free(mini->cmd[i]);
		free(mini->cmd);
	}
	if (mini->input)
		free(mini->input);
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
int		too_args_exit_error(void)
{
	ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
	return (1);
}

int		is_only_digit(char *s)
{
	int i;

	i = -1;
	while (s[++i])
		if (!ft_isdigit(s[i]))
			return (0);
	return (1);
}

int		bad_exit_arg(char *s)
{
	ft_putstr_fd("exit: ", STDERR_FILENO);
	ft_putstr_fd(s, STDERR_FILENO);
	ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
	return (255);
}

int		exit_minishell(char	**splited_inputs, t_mini *mini)
{
	int	return_value;

	return_value = mini->last_return;
	if (splited_inputs)
	{
		if (splited_inputs[1] && !is_only_digit(splited_inputs[1]))
			return_value = bad_exit_arg(splited_inputs[1]);
		else if (splited_inputs[1] && splited_inputs[2])
			return (too_args_exit_error());
		else if (splited_inputs[1])
			return_value = ft_atoi(splited_inputs[1]);
	}
	free_history(&(mini->history));
	free_inputs(mini);
	ft_lstclear(&mini->env, free);
	if (mini->envp)
		ft_free_splited(mini->envp);
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
	char	*str;
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

int		export_error(char *env_var)
{
	ft_putstr_fd("export: \'", STDERR_FILENO);
	ft_putstr_fd(env_var, STDERR_FILENO);
	ft_putstr_fd("\': not a valid identifier\n", STDERR_FILENO);
	return (1);
}
int		is_valid_env_char(char c, int i)
{
	if (i == 0 && ft_isdigit(c))
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
		if ((splited_inputs[i][j] == '=' && j > 0) || !splited_inputs[i][j])
			add_env_var(splited_inputs[i], env, ft_strndup(splited_inputs[i], j));
		else
			mini->last_return = export_error(splited_inputs[i]);
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

int		unset_error(char *env_var)
{
	ft_putstr_fd("unset: \'", STDERR_FILENO);
	ft_putstr_fd(env_var, STDERR_FILENO);
	ft_putstr_fd("\': not a valid identifier\n", STDERR_FILENO);
	return (1);
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
		if (!splited_inputs[i][j])
			delete_env_var(splited_inputs[i], &env);
		else
			mini->last_return = unset_error(splited_inputs[i]);
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
	user_path = ft_strjoin_path("/Users", user);
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

void	cd_error(char *mov, char *error)
{
	ft_putstr_fd("cd: ", STDERR_FILENO);
	ft_putstr_fd(mov, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd(error, STDERR_FILENO);
	ft_putchar_fd('\n', STDERR_FILENO);
}

int		cd_oldpwd(t_mini *mini, char *mov)
{
	free(mov);
	mov = get_env_var("OLDPWD", mini->env);
	if (!*mov)
	{
		free(mov);
		cd_error("OLDPWD", "not set");
		return (1);
	}
	mini->last_return = chdir(mov);
	update_pwd_paths(mini);
	free(mov);
	return (0);
}

int		cd_builtin(t_mini *mini, char *mov)
{
	int		mov_usr;

	mov_usr = 0;
	if (mov == NULL)
	{
		mov = get_user_dir(mini);
		mov_usr = 1;
	}
	else if (ft_strcmp(mov, "-") == 0)
		return (cd_oldpwd(mini, mov));
	mini->last_return = chdir(mov);
	if (!mini->last_return)
		update_pwd_paths(mini);
	else if (mini->last_return == -1)
	{
		cd_error(mov, strerror(errno));
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
		mini->last_return = cd_builtin(mini, splited_inputs[1]);
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

	i = -1;
	str = ft_strdup("");
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
	if (*new != 0 || ft_isalpha(str[*i + 1]))
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
			new = update_input_with_echap(&str, new, i, mini->env);
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

char			*update_input_with_lil_quotes(char **s, char *new, int *i, t_list *env)
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

char			*update_input_with_echap(char **s, char *new, int *i, t_list *env)
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
			new = update_input_with_echap(&s, new, &i, env);
		else if (s[i] == '$' && s[i + 1] && ft_isenvchar(s[i + 1]))
			new = update_input_with_var(&s, new, &i, env);
		else if (s[i] == '\"')
			new = update_input_with_big_quotes(&s, new, &i, mini);
		else if (s[i] == '\'')
			new = update_input_with_lil_quotes(&s, new, &i, env);
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
	char	quote;

	i = 0;
	count = 1;
	while (input[i])
	{
		if (input[i] == ';')
			count += 1;
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
	{
		ft_putstr_fd(file, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(strerror(errno), STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
		return (1);
	}
	close(fd);
	return (0);
}

void	get_fdout_file(t_instructions *instruct, char *s, t_mini *mini)
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
	else
	{
		i += 2;
		method = ">>";
	}
	while (s[i] && (s[i] == ' ' || s[i] == 9))
		i++;
	size = ft_word_size(s + i);
	file = get_real_input(ft_strndup(s + i, size), mini, mini->env);
	instruct->fdout.name = file;
	instruct->fdout.method = method;
	instruct->fdout.is_file = 1;
	create_and_close_file(file, method);
	free(s);
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
	size = ft_word_size(s + i);
	file = get_real_input(ft_strndup(s + i, size), mini, mini->env);
	instruct->fdin.name = file;
	instruct->fdin.method = method;
	instruct->fdin.is_file = 1;
	return (create_and_close_file(file, method));
}

t_list	*ft_lst_input(t_mini *mini, t_instructions *instruc, char *s)
{
	t_list	*cmd;
	int		words_nb;
	int		i;
	int		len;

	if (!s)
		return (NULL);
	cmd = NULL;
	i = -1;
	while (*s)
	{
		while ((*s == ' ' || *s == 9) && *s)
			s++;
		if (!(*s))
			return (cmd);
		len = ft_word_size(s);
		if (*s == '>')
			get_fdout_file(instruc, ft_strndup(s, len), mini);
		else if (*s == '<')
		{
			if (get_fdin_file(instruc, ft_strndup(s, len), mini) == 1)
				return (cmd);
		}
		else
			(ft_lstadd_back(&cmd, ft_lstnew(get_real_input(ft_strndup(s, len), mini, mini->env))));
		s += len;
	}
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
		cmd_tab[i] = ft_strdup((char *)elem->content);
		elem = elem->next;
	}
	cmd_tab[i] = NULL;
	return (cmd_tab);
}

t_history	*ft_historynew(char *input)
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
	elem->fdout.fd = 1;
	elem->fdout.is_file = 0;
	elem->fdout.name = NULL;
	elem->next = NULL;
	return (elem);
}

int		parsing_error(char c)
{
	ft_putstr_fd("syntax error near unexpected token", STDERR_FILENO);
	if (c == '|')
		ft_putstr_fd(" \'|\'\n", STDERR_FILENO);
	return (258);
}

int		get_instructions(t_mini *mini, char *s, t_list *env)
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
		if ((!cmd || !*((char *)cmd->content)) && mini->is_pipe)
			return (parsing_error('|'));
		current->cmds = cmd;
		ft_instruct_add_back(&mini->instructions, current);
		free(instruction);
		s += len;
	}
	return (0);
}

void		make_pipe(t_mini *mini, t_instructions *instruc);

t_list		*ft_lst_cmds(t_mini *mini, char *s, t_list *env)
{
	char	*cmd_input;
	int		cmd_nb;
	int		i;
	int		len;

	if (!s)
		return (NULL);
	cmd_nb = cmd_count(s);
	i = 0;
	while (i < cmd_nb)
	{
		len = ft_cmd_size(s, ';');
		cmd_input = ft_strndup(s, len);
		mini->last_return = get_instructions(mini, cmd_input, env);
		if (mini->last_return)
		{
			free_cmds(mini);
			free(cmd_input);
			set_mini(mini);
			mini->cmds = NULL;
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
			free_cmds(mini);
		}
		free(cmd_input);
		s += len + 1;
		i++;
		set_mini(mini);
	}
	mini->cmds = NULL;
	i = -1;
	return (mini->cmds);
}

void	print_exec_error(char *cmd)
{
	ft_putstr_fd(cmd, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd(strerror(errno), STDERR_FILENO);
	ft_putchar_fd('\n', STDERR_FILENO);
	exit(127);
}

void	exec_cmd(t_mini *mini, char **cmd)
{
	int		status;

	mini->envp = transform_env_lst_in_tab(mini->env);
	if (!*cmd | !**cmd)
		;
	else if (run_builtins(cmd, mini))
		;
	else
	{
		sig_catcher.pid = fork();
		if (sig_catcher.pid == 0)
		{
			if (!ft_strchr(cmd[0], '/'))
				run_bin(cmd, mini);
			else
				execve(cmd[0], cmd, mini->envp);
			print_exec_error(cmd[0]);
		}
		else
		{
			if (0 < waitpid(sig_catcher.pid, &(mini->last_return), 0) && WIFEXITED(mini->last_return))
				mini->last_return = WEXITSTATUS(mini->last_return);
			if (mini->last_return == 2 || mini->last_return == 3)
				mini->last_return += 128;
		}
	}
}

static void redirect(int oldfd, int newfd) {
	if (oldfd != newfd)
	{
		if (dup2(oldfd, newfd) != -1)
			close(oldfd);
		else
			;
	}
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

int		run_bin(char **cmd, t_mini *mini)
{
	char	*path_list;
	char	*tmp;
	char	*path;
	char	*bin;
	int		path_len;

	bin = ft_strdup(cmd[0]);
	path_list = get_env_var("PATH", mini->env);
	while (*path_list)
	{
		path_len = 0;
		while (path_list[path_len] && path_list[path_len] != ':')
			path_len++;
		path = ft_strndup(path_list, path_len);
		free(cmd[0]);
		cmd[0] = ft_strjoin_path(path, bin);
		free(path);
		execve(cmd[0], cmd, mini->envp);
		path_list += path_len;
		if (*path_list == ':')
			path_list += 1;
	}
	ft_putstr_fd(bin, STDERR_FILENO);
	ft_putstr_fd(": command not found\n", STDERR_FILENO);
	exit(127);
	return (0);
}

int		run(t_mini *mini, int fdin, int fdout)
{
	char	**cmd;
	int		status;

	cmd = mini->cmd;
	mini->envp = transform_env_lst_in_tab(mini->env);
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
			run_bin(cmd, mini);
		else
			execve(cmd[0], cmd, mini->envp);
		print_exec_error(cmd[0]);
	}
	exit(0);
	return (1);
}

void	run_piped_child(t_mini *mini,  t_instructions *instruc, t_files_portal *fds)
{
	close(fds->fd[0]);
	if (instruc->fdout.name)
	{
		fds->fdout = open_agreg_file(instruc->fdout.name, instruc->fdout.method);
		dup2(fds->fdout, fds->fd[1]);
		close(fds->fdout);
	}
	else if (!instruc->next)
	{
		close(fds->fd[1]);
		fds->fd[1] = STDOUT_FILENO;
	}
	mini->cmd = get_cmd_tab(instruc->cmds);
	run(mini, fds->fdin, fds->fd[1]);
	free_cmds(mini);
}

void	run_piped_parent(t_mini *mini,  t_instructions *instruc, t_files_portal *fds)
{
	if (0 < waitpid(sig_catcher.pid, &(mini->last_return), 0) && WIFEXITED(mini->last_return))
		mini->last_return = WEXITSTATUS(mini->last_return);
	if (mini->last_return == 2 || mini->last_return == 3)
		mini->last_return += 128;
	if (fds->fd[1] != STDOUT_FILENO)
		close(fds->fd[1]);
	if (fds->fdin != STDIN_FILENO)
		close(fds->fdin);
	if (!instruc->next)
	{
		dup2(mini->stdin_copy, STDIN_FILENO);
		dup2(mini->stdout_copy, STDOUT_FILENO);
	}
	fds->fdin = fds->fd[0];
}
void	make_pipe(t_mini *mini, t_instructions *instruc)
{
	t_files_portal	fds;
	int				status;

	fds.fdin = STDIN_FILENO;
	fds.fdout = STDOUT_FILENO;
	if (!instruc)
		return ;
	while (instruc)
	{
		if (instruc->fdin.name)
		{
			if (fds.fdin != 0)
				close(fds.fdin);
			fds.fdin = open_agreg_file(instruc->fdin.name, instruc->fdin.method);
		}
		if (pipe(fds.fd) == -1 || ((sig_catcher.pid = fork()) < 0))
			return ;
		if (sig_catcher.pid == 0)
			run_piped_child(mini, instruc, &fds);
		else
			run_piped_parent(mini, instruc, &fds);
		instruc = instruc->next;
	}
	close(fds.fd[0]);
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
	int i;

	ft_lstclear(&mini->cmds, free);
	ft_instruclear(&mini->instructions);
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
	if (!(*mini->history->input) && !*buff)
		return ;
	if (!*top && *mini->history->input)
		mini->history->input[ft_strlen(mini->history->input) - 1] = 0;
	else if (*top)
	{
		*top -= 1;
		buff[*top] = 0;
	}
	write(1, "\b \b", 3);
}

void	join_prompt_parts(t_mini *mini, char *buff)
{
	char *tmp;

	tmp = mini->history->input;
	mini->history->input = ft_strjoin(tmp, buff);
	free(tmp);
	ft_bzero(buff, ft_strlen(buff));
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

int		get_lines_nb(t_cursor *cursor, int len)
{
	int	result;

	result = 0;
	if (len - 1 > cursor->max_col - cursor->col)
	{
		result += 1;
		len -= (cursor->max_col - cursor->col);
	}
	else
		return (result);
	while (len > cursor->max_col)
	{
		result += 1;
		len -= cursor->max_col;
	}
	return (result);
}


int		get_line(t_cursor *cursor, int len)
{
	int	result;

	result = 0;
	if (len - 1 > cursor->max_col - cursor->col)
	{
		result += 1;
		len -= (cursor->max_col - cursor->col);
	}
	else
		return (result);
	while (len > cursor->max_col)
	{
		result += 1;
		len -= cursor->max_col;
	}
	if (cursor->line + result - 1 < cursor->max_line)
		return (0);
	return ((cursor->line + result) - cursor->max_line);
}

void	erase_current_prompt(t_mini *mini, int *top, char *buff, t_cursor *cursor)
{
	char	*cm_cap;
	char	*erase_cap;
	int		len;
	char	*buffer;

	cursor->max_col = tgetnum("co");
	cursor->max_line = tgetnum("li");
	cm_cap = tgetstr("cm", NULL);
	len = ft_strlen(mini->history->input);
	cursor->line -= get_line(cursor, len);
	tputs(tgoto(cm_cap, cursor->col - 1 , cursor->line - 1), 1, ft_putchar);
	erase_cap = tgetstr("ce", NULL);
	tputs(erase_cap, 1, ft_putchar);
	if (get_lines_nb(cursor, len))
	{
		tputs(tgoto(cm_cap, 0, cursor->line), 1, ft_putchar);
		erase_cap = tgetstr("cd", NULL);
		tputs(erase_cap, 1, ft_putchar);
		tputs(tgoto(cm_cap, cursor->col - 1 , cursor->line - 1), 1, ft_putchar);
	}
	ft_bzero(mini->history->input, ft_strlen(mini->history->input));
	ft_bzero(buff, ft_strlen(buff));
	*top = 0;
}

void	get_cursor_position(t_cursor *cursor)
{
	char cmd[]="\033[6n";
	char buff[128];
	int len = ft_strlen(cmd);
	ft_bzero(buff, 128);
	write(1, cmd, len);
	read(1, buff, 127);
	int i = 2;
	cursor->line = ft_atoi(&buff[i]);
	while (buff[i] && buff[i] != ';')
		i++;
	i+= 1;
	cursor->col = ft_atoi(&buff[i]);
}

void		add_input_in_history(t_mini *mini, char *input)
{
	t_history	*elem;

	if (mini->history && !*mini->history->input)
	{
		mini->current_hist = mini->history;
		return ;
	}
	elem = ft_historynew(input);
	if (mini->history)
		ft_history_add_front(&mini->history, elem);
	else
		mini->history = elem;
	mini->current_hist = mini->history;
}

void	up_history(t_mini *mini, int *top, char *buff, t_cursor *cursor)
{
	char	*cm_cap;
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
	{
		cm_cap = tgetstr("ic", NULL);
		tputs(cm_cap, 1, ft_putchar);
		write_char_in_prompt(mini, new[i], top, buff);
		cm_cap = tgetstr("ip", NULL);
		tputs(cm_cap, 1, ft_putchar);
	}
	cursor->line -= get_line(cursor, len);
}

void	down_history(t_mini *mini, int *top, char * buff, t_cursor *cursor)
{
	char	*cm_cap;
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
	{
		cm_cap = tgetstr("ic", NULL);
		tputs(cm_cap, 1, ft_putchar);
		write_char_in_prompt(mini, new[i], top, buff);
		cm_cap = tgetstr("ip", NULL);
		tputs(cm_cap, 1, ft_putchar);
	}
	cursor->line -= get_line(cursor, len);
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

void	read_prompt(t_mini *mini)
{
	char		buffchar[3];
	char		buff[128];
	int			top;

	add_input_in_history(mini, NULL);
	mini->current_hist = mini->history;
	top = 0;
	ft_bzero(buff, 128);
	set_mode();
	get_cursor_position(&mini->cursor);
	ft_bzero(buffchar, 3);
	while (read(STDIN_FILENO, buffchar, 3) && sig_catcher.should_run)
		if (!check_prompt_input(mini, &top, buffchar, buff))
			break ;
	write(1, "\n", 1);
	reset_input_mode();
	join_prompt_parts(mini, buff);
	mini->input = ft_strdup(mini->history->input);
	mini->history->is_prompt = 0;
}

int		ft_get_input(t_mini *mini)
{
	char	buffer[2];
	char	*tmp;
	int		size;

	sig_catcher.should_run = 1;
	sig_catcher.pid = -1;
	ft_putstr_fd("\033[0;34mminishell> \033[0m", 1);
	read_prompt(mini);
	if (sig_catcher.should_run == 0)
	{
		mini->last_return = 1;
		dup2(mini->stdin_copy, STDIN_FILENO);
		reset_input_mode();
		ft_bzero(mini->input, ft_strlen(mini->input));
		ft_bzero(mini->history->input, ft_strlen(mini->history->input));
		return(1);
	}
	if (ft_strchr(mini->input, '\n'))
		*(ft_strchr(mini->input, '\n')) = '\0';
	ft_lst_cmds(mini, mini->input, mini->env);
	free(mini->input);
	mini->input = NULL;
	return (1);
}

t_list	*copy_env(char **envp)
{
	t_list	*env;
	t_list	*elem;

	env = NULL;
	elem = NULL;
	while (*envp)
	{
		elem = ft_lstnew(ft_strdup(*envp));
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
		ft_putstr_fd("Quit\n", STDERR_FILENO);
		kill(sig_catcher.pid, SIGQUIT);
	}
}

t_mini	*init_mini(char **envp_tocpy)
{
	t_mini	*mini;

	if (!(mini = malloc(sizeof(t_mini))))
		return (NULL);
	if (!(mini->input = malloc(sizeof(char) * 1)))
		return (NULL);
	*(mini->input) = 0;
	mini->env = copy_env(envp_tocpy);
	mini->stdin_copy = dup(STDIN_FILENO);
	mini->stdout_copy = dup(STDOUT_FILENO);
	mini->instructions = NULL;
	mini->history = NULL;
	mini->last_return = 0;
	set_mini(mini);
	return (mini);
}

int		main(int argc, char **argv, char **envp)
{
	t_mini *mini;
	int		ret;
	char	*term_type;
	char	*cm_cap;
	int		len;

	term_type = getenv("TERM");
	ret = tgetent(NULL, term_type);
	if (!term_type || ret < 1)
	{
		printf("no termtype or pb\n");
		return 1;
	}
	mini = init_mini(envp);
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, sig_handler);
	while (1)
		ft_get_input(mini);
}
