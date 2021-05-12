/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_builtin.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 14:15:38 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/12 14:23:33 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		is_valid_env_char(char c, int i)
{
	if (i == 0 && (ft_isdigit(c) || c == 0))
		return (0);
	else if (ft_isalnum(c) || c == '_')
		return (1);
	return (0);
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
		if (ft_strnstr((char *)current->content, key, key_len)
				== current->content
				&& (*(char *)(current->content + key_len) == '=' ||
					!*(char *)(current->content + key_len)))
			delete_env_var_elem(env, &current, &previous);
		else
		{
			previous = current;
			current = previous->next;
		}
	}
}

int		unset_builtin(t_mini *mini, char **splited_inputs, t_list *env)
{
	int	i;
	int	j;

	i = -1;
	while (splited_inputs[++i])
	{
		mini->last_return = 0;
		j = -1;
		while (splited_inputs[i][++j] &&
				is_valid_env_char(splited_inputs[i][j], j))
			;
		if (!splited_inputs[i][j] && j > 0)
			delete_env_var(splited_inputs[i], &env);
		else
			mini->last_return = print_errors("export",
					splited_inputs[i], "not a valid identifier", 1);
	}
	return (mini->last_return);
}
