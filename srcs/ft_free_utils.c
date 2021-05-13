/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 10:31:23 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/13 10:34:25 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_current_cmd(t_mini *mini, char *cmd_input)
{
	free_mini_tab(mini->envp);
	free_cmds(mini);
	mini->cmds = NULL;
	if (cmd_input)
		free(cmd_input);
	cmd_input = NULL;
	set_mini(mini);
}

void	free_cmds(t_mini *mini)
{
	ft_lstclear(&mini->cmds, free);
	ft_instruclear(&mini->instructions);
	if (mini->bin)
		free(mini->bin);
	free_mini_cmd(mini);
	free(mini->cmds);
	mini->cmds = NULL;
	set_mini(mini);
}
