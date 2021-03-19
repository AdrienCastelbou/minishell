#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <term.h>
#include <unistd.h>
#include "libft.h"
#include <string.h>
#include <errno.h>

typedef struct	s_mini {
		char	***cmds;
		char	**envp;
		char	*input;
		t_list	*env;
}				t_mini;

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
	int	i;

	i = -1;
	while (mini->cmds[++i])
		ft_free_splited(mini->cmds[i]);
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
	return (1);
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
	return (1);
}

int		env_builtin(t_list *env)
{
	while (env)
	{
		printf("%s\n", (char *)(env->content));
		env = env->next;
	}
	return (1);
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
		return (1);
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
		return (1);
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

int		run_builtins(char	**splited_inputs, t_mini *mini)
{
	int		status;

	if (!splited_inputs || !(*splited_inputs))
		return (0);
	status = 0;
	if (ft_strcmp(splited_inputs[0], "echo") == 0)
		status = echo_builtin(splited_inputs);
	else if (ft_strcmp(splited_inputs[0], "cd") == 0)
		status = chdir(splited_inputs[1]);
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
	printf("%d\n", status);
	if (status < 0)
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

static char		*ft_create_w(const char *s, int len, t_list *env)
{
	char	*str;

	if (!(str = (char *)malloc(sizeof(char) * (len + 1))))
		return (NULL);
	str[len] = '\0';
	while (len--)
		str[len] = s[len];
	return (str);
}

static void		ft_free_tab(char **splited, int size)
{
	int i;

	i = -1;
	while (++i < size)
		free(splited[i]);
	free(splited);
}

char			**ft_split_input(char const *s, char c, t_list *env)
{
	int		words_nb;
	char	**splited;
	int		i;
	int		len;

	if (!s)
		return (NULL);
	words_nb = ft_words_count(s, c);
	if (!(splited = (char **)malloc(sizeof(char *) * (words_nb + 1))))
		return (NULL);
	i = -1;
	while (++i < words_nb)
	{
		while (*s == c && *s)
			s++;
		len = ft_word_size(s, c);
		if (!(splited[i] = get_real_input((char *)s, env)))
		{
			ft_free_tab(splited, i);
			return (NULL);
		}
		s += len;
	}
	splited[i] = NULL;
	return (splited);
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

char		***ft_split_cmds(char *s, t_list *env)
{
	int		cmd_nb;
	char	***cmds;
	int		i;
	int		len;

	if (!s)
		return (NULL);
	cmd_nb = cmd_count(s);
	if (!(cmds = (char ***)malloc(sizeof(char **) * (cmd_nb + 1))))
		return (NULL);
	i = 0;
	while (i < cmd_nb)
	{
		len = ft_word_size(s, ';');
		cmds[i] = ft_split_input(ft_strndup(s, len), ' ', env);
		s += len + 1;
		i++;
	}
	cmds[i] = NULL;
	return (cmds);
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
	mini->cmds = NULL;
	mini->input = malloc(sizeof(char) * 1);
	*(mini->input) = 0;
	mini->envp = NULL;
}

int		ft_get_input(t_mini *mini)
{
	char	buffer[128];
	char	*tmp;
	int		size;
	int		status;
	int		i;
	pid_t	pid;

	ft_putstr_fd("\033[0;34mminishell> \033[0m", 1);
	while ( !ft_strchr(mini->input, '\n') && (size = read(STDIN_FILENO, buffer, 128)) > 0)
	{
		buffer[size] = 0;
		tmp = mini->input;
		mini->input = ft_strjoin(tmp, buffer);
	}
	if (ft_strchr(mini->input, '\n'))
		*(ft_strchr(mini->input, '\n')) = '\0';
	mini->cmds = ft_split_cmds(mini->input, mini->env);
	i = 0;
	while (mini->cmds[i])
	{
		if (!*mini->cmds[i])
			;
		else if (run_builtins(mini->cmds[i], mini))
			;
		else
		{
			if (!ft_strchr(mini->cmds[i][0], '/'))
			{
				tmp = mini->cmds[i][0];
				mini->cmds[i][0] = ft_strjoin("/bin/", tmp);
				free(tmp);
			}
			pid = fork();
			if (pid)
				wait(&status);
			else
			{
				mini->envp = transform_env_lst_in_tab(mini->env);
				status = execve(mini->cmds[i][0], mini->cmds[i], mini->envp);
				exit(0);
				free(mini->envp);
			}
		}
			i++;
	}
	free_inputs(mini);
	set_mini(mini);
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
