/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 12:16:25 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/12 12:19:45 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	run_piped_child(t_mini *mini, t_instructions *instruc,
		int fdin, int *pfd)
{
	close(pfd[0]);
	mini->cmd = get_cmd_tab(instruc->cmds);
	run(mini, fdin, pfd[1]);
	exit(127);
}

void	run_piped_parent(t_mini *mini, t_instructions *instruc,
		t_files_portal fds, int pid)
{
	int	status;

	close(fds.pfd[1]);
	close(fds.fdin);
	fds.fdin = fds.pfd[0];
	if (instruc)
		pipe_loop(mini, instruc->next, fds.fdin);
	if (0 < waitpid(pid, &status, 0) && WIFEXITED(status))
	{
		if (!instruc->next)
		{
			mini->last_return = WEXITSTATUS(status);
			if ((mini->last_return == 2 ||
						mini->last_return == 3) && sig_catcher.should_run == 0)
				mini->last_return += 128;
		}
	}
	close(fds.pfd[0]);
	sig_catcher.should_run = 1;
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	dup2(mini->stdin_copy, STDIN_FILENO);
	dup2(mini->stdout_copy, STDOUT_FILENO);
}

void	define_pipe_in_out(t_files_portal *fds, t_instructions *instruc)
{
	if (instruc->fdin.name)
	{
		if (fds->fdin != STDIN_FILENO)
			close(fds->fdin);
		fds->fdin =
			open_agreg_file(instruc->fdin.name, instruc->fdin.method);
	}
	if (instruc->fdout.name)
	{
		fds->fdout =
			open_agreg_file(instruc->fdout.name, instruc->fdout.method);
		dup2(fds->fdout, fds->pfd[1]);
		close(fds->fdout);
	}
	else if (!instruc->next)
	{
		close(fds->pfd[1]);
		fds->pfd[1] = STDOUT_FILENO;
	}
}

void	pipe_loop(t_mini *mini, t_instructions *instruc, int fdin)
{
	t_files_portal	fds;
	int				pid;

	fds.fdin = fdin;
	if (!instruc)
		return ;
	if (pipe(fds.pfd) == -1)
		return ;
	define_pipe_in_out(&fds, instruc);
	if ((pid = fork()) < 0)
		return ;
	sig_catcher.pid = pid;
	if (pid == 0)
		run_piped_child(mini, instruc, fds.fdin, fds.pfd);
	else
		run_piped_parent(mini, instruc, fds, pid);
}

void	make_pipe(t_mini *mini, t_instructions *instruc)
{
	mini->envp = transform_env_lst_in_tab(mini->env);
	mini->path_list = get_env_var("PATH", mini->env);
	pipe_loop(mini, instruc, STDIN_FILENO);
	if (mini->path_list)
		free(mini->path_list);
	mini->path_list = NULL;
}
