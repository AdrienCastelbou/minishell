#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <term.h>
#include <unistd.h>
#include "libft.h"
#include <string.h>
#include <errno.h>


extern char	**environ;

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

void	ft_free_inputs(char **splited_inputs, char *input)
{
	int i;
	i = -1;
	while (splited_inputs[++i])
		free(splited_inputs[i]);
	free(splited_inputs);
	free(input);

}

int		exit_minishell(char	**splited_inputs, char *input, t_list *env)
{
	ft_free_inputs(splited_inputs, input);
	ft_lstclear(&env, free);
	exit(0);
	return (1);
}

int		echo_builtin(char	**splited_inputs, char *input)
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

int		run_builtins(char	**splited_inputs, char *input, t_list *env)
{
	int		status;
	char	buff[128];
	char	*buff_copy;

	status = 0;
	ft_bzero(buff, 128);
	if (ft_strcmp(splited_inputs[0], "echo") == 0)
		status = echo_builtin(splited_inputs, input);
	else if (ft_strcmp(splited_inputs[0], "cd") == 0)
		status = chdir(splited_inputs[1]);
	else if (ft_strcmp(splited_inputs[0], "pwd") == 0)
	{
		if ((buff_copy = getcwd(buff, 128)) != NULL)
			printf("%s\n", buff_copy);
	}
	else if (ft_strcmp(splited_inputs[0], "exit") == 0)
		status = exit_minishell(splited_inputs, input, env);
	else if (ft_strcmp(splited_inputs[0], "env") == 0)
		status = env_builtin(env);
	else
		return (0);
	if (status < 0 || buff_copy == NULL)
		printf("%s: %s: %s\n",splited_inputs[0], strerror(errno), splited_inputs[1]);
	ft_free_inputs(splited_inputs, input);
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
		}
		else if (s[len] == '\'')
		{
			len += 1;
			while (s[len] != '\'' && s[len])
				len++;
		}
		else
			len++;
	}
	printf("%d\n", len);
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

char			*get_real_input(char *s, t_list *env)
{
	int		i;
	char	*tmp;
	char	*dup_s;
	char	*var;
	char	*new;

	new = malloc(sizeof(char) * 1);
	*new = 0;
	i = 0;
	while (s[i] != ' ' && s[i])
	{
		if (s[i] == '$' && s[i + 1] && !ft_strchr("\"\' ", s[i + 1]))
		{
			dup_s = ft_strndup(s, i);
			var = get_env_var(&s[i + 1], env);
			tmp = new;
			new = ft_strjoin(tmp, dup_s);
			free(dup_s);
			free(tmp);
			tmp = new;
			new = ft_strjoin(tmp, var);
			free(tmp);
			while (s[++i] && !ft_strchr("\"\' ", s[i]))
				;
			s = s + i;
			i = 0;
		}
		else if (s[i] == '\"')
		{
			while (s[i] != '\"' && s[i])
			{
				if (s[i] == '$' && s[i + 1] && !ft_strchr("\"\' ", s[i + 1]))
				{
					i++;
				}
				else
					i++;
			}
			i++;
		}
		else if (s[i] == '\'')
		{
			i++;
		}
		else
			i++;
	}
	dup_s = ft_strndup(s, i);
	tmp = new;
	new = ft_strjoin(tmp, dup_s);
	free(tmp);
	free(dup_s);
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
		if (!(splited[i] = ft_create_w(s, len, env)))
		{
			ft_free_tab(splited, i);
			return (NULL);
		}
		s += len;
	}
	splited[i] = NULL;
	return (splited);
}

int		ft_get_input(t_list *envrmt)
{
	char	buffer[4];
	char	*tmp;
	char	*input;
	char	**splited_inputs;
	int		size;
	int		status;
	pid_t	pid;

	input = malloc(1);
	*input = 0;
	t_list *env = envrmt;
	ft_putstr_fd("\033[0;34mminishell> \033[0m", 1);
	while ( !ft_strchr(input, '\n') && (size = read(STDIN_FILENO, buffer, 4)) > 0)
	{
		buffer[size] = 0;
		tmp = input;
		input = ft_strjoin(tmp, buffer);
	}
	if (ft_strchr(input, '\n'))
		*(ft_strchr(input, '\n')) = 0;
	char *test;
	test = get_real_input(input, env);
	printf("%s\n", test);
	free(test);
	splited_inputs = ft_split(input, ' ');
	if (!splited_inputs[0])
		return (1);
	if (run_builtins(splited_inputs, input, env))
		return (1);
	if (!ft_strchr(splited_inputs[0], '/'))
	{
		tmp = splited_inputs[0];
		splited_inputs[0] = ft_strjoin("/bin/", tmp);
		free(tmp);
	}
	pid = fork();
	if (pid)
		wait(&status);
	else
		status = execve(splited_inputs[0], splited_inputs, NULL);
	ft_free_inputs(splited_inputs, input);
	return (1);
}

t_list	*copy_env(void)
{
	t_list	*env;
	t_list	*elem;
	char	**extern_env;

	env = NULL;
	elem = NULL;
	extern_env = environ;
	while (*extern_env)
	{
		elem = ft_lstnew(ft_strdup(*extern_env));
		ft_lstadd_back(&env, elem);
		extern_env++;
	}
	return (env);
}

int		main(int argc, char **argv)
{
	t_list *env;

	env = copy_env();
	while (1)
		ft_get_input(env);
}
