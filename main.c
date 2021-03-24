#include "minishell.h"

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
	while (mini->cmd[++i])
		free(mini->cmd[i]);
	free(mini->cmd);
	if (mini->input)
		free(mini->input);
}

int		exit_minishell(char	**splited_inputs, t_mini *mini)
{
	free_inputs(mini);
	ft_lstclear(&mini->env, free);
	if (mini->envp)
		ft_free_splited(mini->envp);
	free(mini);
	exit(0);
	return (0);
}

int		echo_builtin(char	**splited_inputs)
{
	int	i;
	int	is_flag;
	i = 0;
	is_flag = 0;
	if (ft_strcmp(splited_inputs[1], "-n") == 0)
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
		printf("%s\n", (char *)(env->content));
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
				&& *(char *)(begin->content + key_len) == '=')
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

int		export_builtin(char	**splited_inputs, t_list *env)
{
	int	i;
	int	j;

	i = -1;
	while (splited_inputs[++i])
	{
		j = -1;
		while (splited_inputs[i][++j] && splited_inputs[i][j] != '=')
		{
			if (splited_inputs[i][j] == ' ')
				break;
		}
		if ((splited_inputs[i][j] == '=' && j > 0) || !splited_inputs[i][j])
			add_env_var(splited_inputs[i], env, ft_strndup(splited_inputs[i], j));
		else
			printf("export: \'%s\': not a valid identifier\n", splited_inputs[i]);
	}
		return (0);
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
				&& *(char *)(current->content + key_len) == '=')
		{
			if (!previous)
				*env = current->next;
			else
				previous->next = current->next;
			ft_lstdelone(current, free);
			if (previous)
				current = previous->next;
			else
				current = *env;
		}
		else
		{
			previous = current;
			current = previous->next;
		}
	}
}

int		unset_builtin(char	**splited_inputs, t_list *env)
{
	int	i;
	int	j;

	i = -1;
	while (splited_inputs[++i])
	{
		j = -1;
		while (splited_inputs[i][++j] && splited_inputs[i][j] != '=')
		{
			if (splited_inputs[i][j] == ' ')
				break;
		}
		if (!splited_inputs[i][j])
			delete_env_var(splited_inputs[i], &env);
		else
			printf("export: \'%s\': not a valid identifier\n", splited_inputs[i]);
	}
		return (0);
}

int		pwd_builtin(void)
{
	char	buff[128];
	char	*buff_copy;
	int		status;

	status = 0;
	ft_bzero(buff, 128);
	if ((buff_copy = getcwd(buff, 128)) != NULL)
		printf("%s\n", buff_copy);
	else
		status = 1;
	return (status);
}

int		cd_builtin(t_mini *mini, char *mov)
{
	int		status;
	char	buff[128];
	char	*pwd[2];

	ft_bzero(buff, 128);
	status = chdir(mov);
	if (!status)
	{
		getcwd(buff, 128);
		pwd[0] = ft_strjoin("PWD=", buff);
		pwd[1] = NULL;
		export_builtin(pwd, mini->env);
		free(pwd[0]);
	}
	return (status);
}

int		run_builtins(char	**splited_inputs, t_mini *mini)
{
	int		status;

	if (!splited_inputs || !(*splited_inputs))
		return (0);
	status = 0;
	if (ft_strcmp(splited_inputs[0], "echo") == 0)
		status = echo_builtin(splited_inputs);
	else if (ft_strcmp(splited_inputs[0], "cd") == 0)
		status = cd_builtin(mini, splited_inputs[1]);
	else if (ft_strcmp(splited_inputs[0], "pwd") == 0)
		status = pwd_builtin();
	else if (ft_strcmp(splited_inputs[0], "export") == 0)
			status = export_builtin(&splited_inputs[1], mini->env);
	else if (ft_strcmp(splited_inputs[0], "unset") == 0)
			status = unset_builtin(&splited_inputs[1], mini->env);
	else if (ft_strcmp(splited_inputs[0], "exit") == 0)
		status = exit_minishell(splited_inputs, mini);
	else if (ft_strcmp(splited_inputs[0], "env") == 0)
		status = env_builtin(mini->env);
	else
		return (0);
	if (status != 0)
		printf("%s: %s: %s\n",splited_inputs[0], strerror(errno), splited_inputs[1]);
	return (1);
}

static int		ft_word_size(const char *s, char c)
{
	int		len;

	len = 0;
	while (s[len] != c && s[len])
	{
		if (s[len] == '\"')
		{
			len += 1;
			while (s[len] != '\"' && s[len])
				len++;
			len += 1;
		}
		else if (s[len] == '\'')
		{
			len += 1;
			while (s[len] != '\'' && s[len])
				len++;
			len += 1;
		}
		else
			len++;
	}
	return (len);
}

static int		ft_words_count(char const *s, char c)
{
	int		count;
	int		len;

	count = 0;
	while (*s)
	{
		while (*s == c)
			s++;
		if ((len = ft_word_size(s, c)))
		{
			s += len;
			count++;
		}
	}
	return (count);
}

char			*get_env_var(const char *s, t_list *env)
{
	int		i;
	char	*variable;
	char	*str;

	i = -1;
	str = ft_strdup("");
	while (s[++i] && !ft_strchr("\"\' ", s[i]))
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
	while (str[++(*i)] && !ft_strchr("\"\' ", str[*i]))
		;
	*s = *s + *i;
	*i = 0;
	return (new);
}

char			*update_input_with_big_quotes(char **s, char *new, int *i, t_list *env)
{
	char *str;

	new = join_input_parts(*s, new, *i);
	*s = *s + *i + 1;
	str = *s;
	*i = 0;
	while (str[*i] != '\"' && str[*i])
	{
		if (str[*i] == '$' && str[*i + 1] && !ft_strchr("\"\' ", str[*i + 1]))
			new = update_input_with_var(&str, new, i, env);
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
	char *str;

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

char			*get_real_input(char *s, t_list *env)
{
	int		i;
	char	*new;

	new = malloc(sizeof(char) * 1);
	*new = 0;
	i = 0;
	while (s[i] != ' ' && s[i])
	{
		if (s[i] == '$' && s[i + 1] && !ft_strchr("\"\' ", s[i + 1]))
			new = update_input_with_var(&s, new, &i, env);
		else if (s[i] == '\"')
			new = update_input_with_big_quotes(&s, new, &i, env);
		else if (s[i] == '\'')
			new = update_input_with_lil_quotes(&s, new, &i, env);
		else
			i++;
	}
	new = join_input_parts(s, new, i);
	return (new);
}

int		cmd_count(char *input)
{
	int	i;
	int	count;

	count = 1;
	i = -1;
	while (input[++i])
	{
		if (input[i] == '\"')
			while (input[++i] && input[i] != '\"')
				;
		else if (input[i] == '\'')
		while (input[++i] && input[i] != '\'')
			;
		else if (input[i] == ';')
			count += 1;
	}
	return (count);
}


t_list	*ft_lst_input(char *s, char c, t_list *env)
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
		while (*s == c && *s)
			s++;
		len = ft_word_size(s, c);
		(ft_lstadd_back(&cmd, ft_lstnew(get_real_input((char *)s, env))));
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
		len = ft_word_size(s, ';');
		cmd_input = ft_strndup(s, len);
		mini->cmds = ft_lst_input(cmd_input, ' ', env);
		mini->cmd = get_cmd_tab(mini->cmds);
		run_cmd(mini, mini->cmd);
		free_cmds(mini);
		free(cmd_input);
		s += len + 1;
		i++;
	}
	mini->cmds = NULL;
	i = -1;
	return (mini->cmds);
}

char	**transform_env_lst_in_tab(t_list *env)
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
		envp[i] = ft_strdup((char *)env->content);
		env = env->next;
	}
	envp[i] = NULL;
	return (envp);
}

void	set_mini(t_mini *mini)
{
	mini->envp = NULL;
	mini->cmds = NULL;
}

void	free_cmds(t_mini *mini)
{
	int i;
	t_list *elem;

	elem = mini->cmds;
	ft_lstclear(&mini->cmds, free);
	free(mini->cmds);
	mini->cmds = NULL;
	i = -1;
	while (mini->cmd[++i])
		free(mini->cmd[i]);
	free(mini->cmd);
	set_mini(mini);
}

void	run_cmd(t_mini *mini, char **cmd)
{
	char	*tmp;
	int		pid;
	int		status;

	if (!*cmd)
		;
	else if (run_builtins(cmd, mini))
		;
	else
	{
		if (!ft_strchr(cmd[0], '/'))
		{
			tmp = cmd[0];
			cmd[0] = ft_strjoin("/bin/", tmp);
			free(tmp);
		}
		pid = fork();
		if (pid)
			wait(&status);
		else
		{
			mini->envp = transform_env_lst_in_tab(mini->env);
			status = execve(cmd[0], cmd, mini->envp);
			exit(0);
			free(mini->envp);
		}
	}
}

int		ft_get_input(t_mini *mini)
{
	char	buffer[128];
	char	*tmp;
	int		size;

	ft_putstr_fd("\033[0;34mminishell> \033[0m", 1);
	while ( !ft_strchr(mini->input, '\n') && (size = read(STDIN_FILENO, buffer, 128)) > 0)
	{
		buffer[size] = 0;
		tmp = mini->input;
		mini->input = ft_strjoin(tmp, buffer);
	}
	if (ft_strchr(mini->input, '\n'))
		*(ft_strchr(mini->input, '\n')) = '\0';
	ft_lst_cmds(mini, mini->input, mini->env);
	free(mini->input);
	mini->input = malloc(sizeof(char) * 1);
	*(mini->input) = 0;
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

t_mini	*init_mini(char **envp_tocpy)
{
	t_mini	*mini;

	if (!(mini = malloc(sizeof(t_mini))))
		return (NULL);
	mini->input = malloc(sizeof(char) * 1);
	*(mini->input) = 0;
	mini->env = copy_env(envp_tocpy);
	set_mini(mini);
	return (mini);
}

int		main(int argc, char **argv, char **envp)
{
	t_mini *mini;

	mini = init_mini(envp);
	while (1)
		ft_get_input(mini);
}
