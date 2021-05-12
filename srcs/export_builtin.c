/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_builtin.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 14:26:28 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/12 15:30:44 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_switch_strs(char **tab_var, int i, int j)
{
	char	*tmp;

	tmp = tab_var[i];
	tab_var[i] = tab_var[j];
	tab_var[j] = tmp;
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
				&& (*(char *)(begin->content + key_len) == '=' ||
					!*(char *)(begin->content + key_len)))
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

void	print_export_var(t_list *env)
{
	char	**tab_var;
	int		i;
	int		j;

	tab_var = transform_env_lst_in_tab(env);
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

int		export_builtin(t_mini *mini, char **splited_inputs, t_list *env)
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
		while (splited_inputs[i][++j] &&
				is_valid_env_char(splited_inputs[i][j], j))
			;
		if (j > 0 && (splited_inputs[i][j] == '=' ||
					(!splited_inputs[i][j])))
			add_env_var(splited_inputs[i], env,
					ft_strndup(splited_inputs[i], j));
		else
			mini->last_return = print_errors("export",
					splited_inputs[i], "not a valid identifier", 1);
	}
	return (mini->last_return);
}
