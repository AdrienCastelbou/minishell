/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_real_input_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 14:49:45 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/12 14:51:37 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
			break ;
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
