/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_mini.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 10:34:56 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/14 09:46:50 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	set_mini(t_mini *mini)
{
	mini->cmd = NULL;
	mini->envp = NULL;
	mini->cmds = NULL;
	mini->bin = NULL;
	mini->path_list = NULL;
	mini->is_pipe = 0;
	mini->is_fd_err = 0;
}

t_mini	*init_mini(t_list *env)
{
	t_mini	*mini;

	if (!(mini = malloc(sizeof(t_mini))))
		return (NULL);
	mini->input = NULL;
	mini->env = env;
	mini->stdin_copy = dup(STDIN_FILENO);
	mini->stdout_copy = dup(STDOUT_FILENO);
	mini->instructions = NULL;
	mini->history = NULL;
	mini->last_return = 0;
	mini->cm_cap = tgetstr("cm", NULL);
	mini->dc_cap = tgetstr("dc", NULL);
	mini->le_cap = tgetstr("le", NULL);
	mini->ce_cap = tgetstr("ce", NULL);
	set_mini(mini);
	if (mini->stdin_copy == -1 || mini->stdout_copy == -1
			|| !mini->cm_cap || !mini->dc_cap || !mini->le_cap || !mini->ce_cap)
		error_init_mini(mini);
	return (mini);
}

t_list	*set_basic_env(void)
{
	char	*value;
	char	*pwd;
	char	*envp[5];
	t_list	*env;

	value = getcwd(NULL, 0);
	if (!value)
		return (NULL);
	pwd = ft_strjoin("PWD=", value);
	if (!pwd)
		return (NULL);
	envp[0] = pwd;
	envp[1] = "SHLVL=0";
	envp[2] = "TERM=xterm-256color";
	envp[3] = "PATH=/bin";
	envp[4] = NULL;
	env = copy_env(envp);
	free(value);
	free(pwd);
	return (env);
}
