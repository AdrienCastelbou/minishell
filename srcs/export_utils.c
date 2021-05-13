/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:11:55 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/13 14:12:34 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	concat_env_var(char *env_var, t_list *env, char *key, int start)
{
	char	*var1;
	char	*var2;
	char	*concat;

	var1 = get_env_var(key, env);
	var2 = ft_strdup(env_var + start);
	if (!var1)
		concat = var2;
	else if (!var2)
		concat = var1;
	else
	{
		concat = ft_strjoin(var1, var2);
		free(var1);
		free(var2);
	}
	var1 = ft_strjoin(key, "=");
	var2 = ft_strjoin(var1, concat);
	add_env_var(var2, env, key);
	if (var1)
		free(var1);
	if (var2)
		free(var2);
	if (concat)
		free(concat);
}

int		is_valid_export_decla(char **splited_inputs, int i, int j)
{
	if (j > 0 && (splited_inputs[i][j] == '=' ||
				(!splited_inputs[i][j])))
		return (1);
	return (0);
}

int		is_valid_concat_decla(char **splited_inputs, int i, int j)
{
	if (j > 0 && splited_inputs[i][j] == '+'
				&& splited_inputs[i][j + 1] == '=')
		return (1);
	return (0);
}
