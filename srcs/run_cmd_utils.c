/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_cmd_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 10:28:56 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/13 11:28:39 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	redirect(int oldfd, int newfd)
{
	if (oldfd != newfd)
		if (dup2(oldfd, newfd) != -1)
			close(oldfd);
}

int		run_builtins(char **splited_inputs, t_mini *mini)
{
	if (!splited_inputs || !(*splited_inputs))
		return (0);
	if (ft_strcmp(splited_inputs[0], "echo") == 0)
		mini->last_return = echo_builtin(splited_inputs);
	else if (ft_strcmp(splited_inputs[0], "cd") == 0)
		mini->last_return = cd_builtin(mini, splited_inputs[1], splited_inputs);
	else if (ft_strcmp(splited_inputs[0], "pwd") == 0)
		mini->last_return = pwd_builtin(mini);
	else if (ft_strcmp(splited_inputs[0], "export") == 0)
		mini->last_return = export_builtin(mini, &splited_inputs[1], mini->env);
	else if (ft_strcmp(splited_inputs[0], "unset") == 0)
		mini->last_return = unset_builtin(mini, &splited_inputs[1], mini->env);
	else if (ft_strcmp(splited_inputs[0], "exit") == 0)
		mini->last_return = exit_minishell(splited_inputs, mini);
	else if (ft_strcmp(splited_inputs[0], "env") == 0)
		mini->last_return = env_builtin(mini->env);
	else
		return (0);
	return (1);
}
