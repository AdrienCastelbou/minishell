/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 09:56:27 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/13 09:59:01 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		if (*s1 != *s2)
			return (*s1 - *s2);
		s1++;
		s2++;
	}
	return (*s1 - *s2);
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
				break ;
			i++;
		}
		env = env->next;
	}
	envp[i] = NULL;
	return (envp);
}

char	**get_cmd_tab(t_list *cmd)
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
				break ;
		}
		else
			--i;
		elem = elem->next;
	}
	cmd_tab[i] = NULL;
	return (cmd_tab);
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
