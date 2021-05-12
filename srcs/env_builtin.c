/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 14:36:39 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/12 14:37:24 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
