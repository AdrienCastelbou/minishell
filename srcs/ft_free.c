/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 15:21:12 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/14 09:49:22 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_free_splited(char **splited_inputs)
{
	int i;

	i = -1;
	while (splited_inputs[++i])
		free(splited_inputs[i]);
	free(splited_inputs);
}

void	free_mini_cmd(t_mini *mini)
{
	int i;

	i = -1;
	if (mini->cmd)
	{
		while (mini->cmd[++i])
		{
			free(mini->cmd[i]);
			mini->cmd[i] = NULL;
		}
		free(mini->cmd);
		mini->cmd = NULL;
	}
}

void	free_mini_tab(char **strs)
{
	int i;

	i = -1;
	if (strs)
	{
		while (strs[++i])
		{
			free(strs[i]);
			strs[i] = NULL;
		}
		free(strs);
		strs = NULL;
	}
}

void	free_inputs(t_mini *mini)
{
	if (!mini)
		return ;
	if (mini->cmd)
		free_mini_tab(mini->cmd);
	mini->cmd = NULL;
	if (mini->input)
		free(mini->input);
	mini->input = NULL;
}

void	free_mini(t_mini *mini)
{
	free_history(&(mini->history));
	free_inputs(mini);
	ft_lstclear(&mini->env, free);
	if (mini->envp)
		ft_free_splited(mini->envp);
	if (mini->instructions)
		ft_instruclear(&mini->instructions);
	if (mini->bin)
		free(mini->bin);
	if (mini->path_list)
		free(mini->path_list);
	close(mini->stdin_copy);
	close(mini->stdout_copy);
	free(mini);
}
