/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 14:37:38 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/13 11:30:18 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		pwd_builtin(t_mini *mini)
{
	char	*pwd;
	int		status;

	status = 0;
	pwd = NULL;
	pwd = get_env_var("PWD", mini->env);
	if (pwd != NULL)
	{
		ft_putstr_fd(pwd , STDOUT_FILENO);
		ft_putchar_fd('\n', STDOUT_FILENO);
		free(pwd);
	}
	else
		status = 1;
	return (status);
}
