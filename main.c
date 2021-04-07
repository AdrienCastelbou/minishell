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
	close(mini->stdin_copy);
	close(mini->stdout_copy);
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
	ft_putstr_fd("declare -x ", 1);
	ft_putstr_fd(key, 1);
	if (str[key_len] == '=')
		printf("=\"%s\"\n", str + key_len + 1);
	else
		write(1, "\n", 1);
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

int		export_builtin(char	**splited_inputs, t_list *env)
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
		j = -1;
		while (splited_inputs[i][++j] && ft_isalnum(splited_inputs[i][j]))
			;
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
				&& (*(char *)(current->content + key_len) == '=' || !*(char *)(current->content + key_len)))
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
		while (splited_inputs[i][++j] && ft_isalnum(splited_inputs[i][j]))
			;
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
	while (s[len] && s[len] == ' ')
		len++;
	len += ft_word_size(s + len, ' ');
	return (len);
}

static int		ft_word_size(const char *s, char c)
{
	int		len;
	int		echap;

	echap = 0;
	len = 0;
	if ((s[len] == '>' || s[len] == '<') && c == ' ')
		return (ft_get_fd_token(s));
	while (s[len])
	{
		if (echap && s[len])
			echap = 0;
		else if (s[len] == c)
			return (len);
		else if (c == ' ' && (s[len] == '>' || s[len] == '<'))
			return (len);
		else if (s[len] == '\"')
			while (s[++len] != '\"' && s[len])
				;
		else if (s[len] == '\'')
			while (s[++len] != '\'' && s[len])
				;
		else if (s[len] == '\\')
			echap = 1;
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
	while (s[++i] && ft_isalnum(s[i]))
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
	while (str[++(*i)] && ft_isalnum(str[*i]))
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
		if (str[*i] == '\\' && (str[*i + 1] == '$' || str[*i + 1] == '\"' || str[*i + 1] == '\\'))
			new = update_input_with_echap(&str, new, i, env);
		else if (str[*i] == '$' && str[*i + 1] && ft_isalnum(str[*i + 1]))
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

char			*get_real_input(char *s, t_list *env)
{
	int		i;
	char	*new;
	char	*str;

	str = s;
	new = malloc(sizeof(char) * 1);
	*new = 0;
	i = 0;
	while (s[i] && s[i] != ' ')
	{
		if (s[i] == '\\')
			new = update_input_with_echap(&s, new, &i, env);
		else if (s[i] == '$' && s[i + 1] && ft_isalnum(s[i + 1]))
			new = update_input_with_var(&s, new, &i, env);
		else if (s[i] == '\"')
			new = update_input_with_big_quotes(&s, new, &i, env);
		else if (s[i] == '\'')
			new = update_input_with_lil_quotes(&s, new, &i, env);
		else
			i++;
	}
	new = join_input_parts(s, new, i);
	free(str);
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
		printf("%s\n", strerror(errno));
		return (1);
	}
	close(fd);
	return (0);
}

void	get_fdout_file(t_instructions *instruct, char *s, t_list *env)
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
	while (s[i] && s[i] == ' ')
		i++;
	size = ft_word_size(s + i, ' ');
	file = get_real_input(ft_strndup(s + i, size), env);
	instruct->fdout.name = file;
	instruct->fdout.method = method;
	instruct->fdout.is_file = 1;
	create_and_close_file(file, method);
	free(s);
}

int		get_fdin_file(t_instructions *instruct, char *s, t_list *env)
{
	char	*method;
	char	*file;
	int		size;
	int		i;

	method = "<";
	i = 1;
	while (s[i] && s[i] == ' ')
		i++;
	size = ft_word_size(s + i, ' ');
	file = get_real_input(ft_strndup(s + i, size), env);
	instruct->fdin.name = file;
	instruct->fdin.method = method;
	instruct->fdin.is_file = 1;
	create_and_close_file(file, method);
	return (0);
}


int		open_file_in_stdin(t_mini *mini, char *s, t_list *env)
{
	char	*file;
	int		size;
	int		i;
	int		fd;

	i = 1;
	while (s[i] && s[i] == ' ')
		i++;
	size = ft_word_size(s + i, ' ');
	file = get_real_input(ft_strndup(s + i, size), env);
	if ((fd = open(file, O_RDONLY)) < 0)
	{
		ft_putstr_fd(file, STDERR_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		free(file);
		free(s);
		return (1);
	}
	if (mini->current_stdin != 0)
		close(mini->current_stdin);
	mini->current_stdin = fd;
	free(file);
	free(s);
	return (0);
}

t_list	*ft_lst_input(t_instructions *instruc, char *s, char c, t_list *env)
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
		if (!(*s))
			return (cmd);
		len = ft_word_size(s, c);
		if (*s == '>')
			get_fdout_file(instruc, ft_strndup(s, len), env);
		else if (*s == '<')
			get_fdin_file(instruc, ft_strndup(s, len), env);
		else
			(ft_lstadd_back(&cmd, ft_lstnew(get_real_input(ft_strndup(s, len), env))));
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
void		get_instructions(t_mini *mini, char *s, t_list *env)
{
	char			*instruction;
	t_instructions	*current;
	t_list			*cmd;
	int				len;

	mini->instructions = NULL;
	if (*s == '|')
		return ;
	while (*s)
	{
		current = ft_instructnew(NULL);
		len = ft_word_size(s, '|');
		instruction = ft_strndup(s, len);
		cmd = ft_lst_input(current, instruction, ' ', env);
		current->cmds = cmd;
		ft_instruct_add_back(&mini->instructions, current);
		free(instruction);
		s += len;
		if (*s == '|')
			s += 1;
	}
}

void		make_pipe(t_mini *mini, t_instructions *instruc);

t_list		*ft_lst_cmds(t_mini *mini, char *s, t_list *env)
{
	char	*cmd_input;
	int		cmd_nb;
	int		i;
	int		len;
	t_instructions *current;

	if (!s)
		return (NULL);
	cmd_nb = cmd_count(s);
	i = 0;
	while (i < cmd_nb)
	{
		len = ft_word_size(s, ';');
		cmd_input = ft_strndup(s, len);
		get_instructions(mini, cmd_input, env);
		make_pipe(mini, mini->instructions);
		/*current = mini->instructions;
		while (current)
		{
			mini->cmd = get_cmd_tab(current->cmds);
			run_cmd(mini, mini->cmd);
			free_cmds(mini);
			current = current->next;
		}
		free(cmd_input);*/
		s += len + 1;
		i++;
	}
	mini->cmds = NULL;
	i = -1;
	return (mini->cmds);
}

void	exec_cmd(t_mini *mini, char **cmd)
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

static void redirect(int oldfd, int newfd) {
	if (oldfd != newfd) {
	if (dup2(oldfd, newfd) != -1)
		close(oldfd); /* successfully redirected */
	else
		;
  }
}

int		run(t_mini *mini, int pid, int fdin, int fdout)
{
	char	*tmp;
	char	**cmd;
	int		status;

	cmd = mini->cmd;
	redirect(fdin, STDIN_FILENO);   /* <&in  : child reads from in */
	redirect(fdout, STDOUT_FILENO);
	if (!*cmd)
		;
	else if (run_builtins(cmd, mini))
		;
	else
	{
		if (!ft_strchr(cmd[0], '/'))
		{
			tmp = cmd[0];
			cmd[0] = ft_strjoin("/usr/bin/", tmp);
			//free(tmp);
		}
		mini->envp = transform_env_lst_in_tab(mini->env);
		status = execve(cmd[0], cmd, mini->envp);
		if (!ft_strchr(tmp, '/'))
			cmd[0] = ft_strjoin("/bin/", tmp);
		status = execve(cmd[0], cmd, mini->envp);
		free(mini->envp);
		exit(0);
	}
	if (!pid)
		exit(0);
	return (1);
}

void	make_pipe(t_mini *mini, t_instructions *instruc)
{
	int	fd[2];
	int	pid;
	int	fdin;
	int	fdout;
	int	status;

	fdin = STDIN_FILENO;
	fdout = STDOUT_FILENO;
	if (!instruc)
		return ;
	while (instruc->next)
	{
		if (instruc->fdin.name)
		{
			if (fdin != 0)
				close(fdin);
			fdin = open_agreg_file(instruc->fdin.name, instruc->fdin.method);
		}
		if (pipe(fd) == -1)
			return ;
		else if ((pid = fork()) < 0)
			return ;
		if (pid == 0)
		{
			close(fd[0]);
			if (instruc->fdout.name)
			{
				fdout = open_agreg_file(instruc->fdout.name, instruc->fdout.method);
				dup2(fdout, fd[1]);
				close(fdout);
			}
			mini->cmd = get_cmd_tab(instruc->cmds);
			run(mini, pid, fdin, fd[1]);
			free_cmds(mini);
			return ;
		}
		else
		{
			waitpid(pid, &status, -1);
			close(fd[1]);
			close(fdin);
			fdin = fd[0];
		}
		instruc = instruc->next;
	}
	if (instruc && instruc->fdin.name)
	{
		if (fdin != 0)
			close(fdin);
		fdin = open_agreg_file(instruc->fdin.name, instruc->fdin.method);
	}
	if (instruc && instruc->fdout.name)
		fdout = open_agreg_file(instruc->fdout.name, instruc->fdout.method);
	else
		fdout = STDOUT_FILENO;
	mini->cmd = get_cmd_tab(instruc->cmds);
	run(mini, pid, fdin, fdout);
	dup2(mini->stdin_copy, STDIN_FILENO);
	dup2(mini->stdout_copy, STDOUT_FILENO);
	free_cmds(mini);
}

/*
void	run_cmd(t_mini *mini, char **cmd)
{
	dup2(mini->current_stdin, STDIN_FILENO);
	mini->current_fd = ft_fdlast(mini->fds)->fd;
	dup2(mini->current_fd, STDOUT_FILENO);
	exec_cmd(mini, cmd);
	dup2(mini->stdin_copy, STDIN_FILENO);
	dup2(mini->stdout_copy, STDOUT_FILENO);
}*/


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
	mini->envp = NULL;
	mini->cmds = NULL;
	mini->current_stdin = 0;
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

void	free_cmds(t_mini *mini)
{
	int i;
	t_list *elem;

	elem = mini->cmds;
	ft_lstclear(&mini->cmds, free);
	ft_fdclear(&mini->fds->next);
	free(mini->cmds);
	mini->cmds = NULL;
	i = -1;
	while (mini->cmd[++i])
		free(mini->cmd[i]);
	free(mini->cmd);
	set_mini(mini);
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
	if (!(mini->input = malloc(sizeof(char) * 1)))
		return (NULL);
	*(mini->input) = 0;
	mini->env = copy_env(envp_tocpy);
	if (!(mini->fds = malloc(sizeof(t_fds))))
		return (NULL);
	mini->fds->fd = 1;
	mini->fds->next = NULL;
	mini->stdin_copy = dup(STDIN_FILENO);
	mini->stdout_copy = dup(STDOUT_FILENO);
	mini->instructions = NULL;
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
