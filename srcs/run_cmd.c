/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_cmd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 12:32:40 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/14 09:09:51 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	run_exec(t_mini *mini, char **cmd)
{
	char	*path_list;

	path_list = get_env_var("PATH", mini->env);
	g_sig_catcher.pid = fork();
	if (g_sig_catcher.pid == 0)
	{
		if (!ft_strchr(cmd[0], '/'))
			run_bin(cmd, mini, path_list);
		else
			execve(cmd[0], cmd, mini->envp);
		exit(print_errors(cmd[0], strerror(errno), NULL, 127));
	}
	else
	{
		if (0 < waitpid(g_sig_catcher.pid, &(mini->last_return), 0)
				&& WIFEXITED(mini->last_return))
			mini->last_return = WEXITSTATUS(mini->last_return);
		if ((mini->last_return == 2 || mini->last_return == 3)
				&& g_sig_catcher.should_run <= 0)
			mini->last_return += 128;
		if (g_sig_catcher.should_run == -1)
			ft_putstr_fd("Quit\n", STDERR_FILENO);
		if (path_list)
			free(path_list);
	}
}

void	exec_cmd(t_mini *mini, char **cmd)
{
	mini->envp = transform_env_lst_in_tab(mini->env);
	if (!cmd || !*cmd || !**cmd)
		;
	else if (run_builtins(cmd, mini))
		;
	else
		run_exec(mini, cmd);
}

int		run_bin(char **cmd, t_mini *mini, char *path_list)
{
	char	*path;
	int		path_len;

	mini->bin = ft_strdup(cmd[0]);
	while (path_list && *path_list)
	{
		path_len = 0;
		while (path_list[path_len] && path_list[path_len] != ':')
			path_len++;
		path = ft_strndup(path_list, path_len);
		free(cmd[0]);
		cmd[0] = ft_strjoin_path(path, mini->bin);
		free(path);
		execve(cmd[0], cmd, mini->envp);
		path_list += path_len;
		if (*path_list == ':')
			path_list += 1;
	}
	ft_putstr_fd("\U0000274C minishell: ", STDERR_FILENO);
	ft_putstr_fd(mini->bin, STDERR_FILENO);
	ft_putstr_fd(": command not found\n", STDERR_FILENO);
	exit(127);
	return (0);
}

int		run(t_mini *mini, int fdin, int fdout)
{
	char	**cmd;

	cmd = mini->cmd;
	if (fdin == -1)
		exit(1);
	redirect(fdin, STDIN_FILENO);
	redirect(fdout, STDOUT_FILENO);
	if (!*cmd || !**cmd)
		;
	else if (run_builtins(cmd, mini))
		;
	else
	{
		if (!ft_strchr(cmd[0], '/'))
			run_bin(cmd, mini, mini->path_list);
		else
			execve(cmd[0], cmd, mini->envp);
		print_errors(cmd[0], strerror(errno), NULL, 127);
		exit(127);
	}
	exit(mini->last_return);
	return (1);
}

void	run_cmd(t_mini *mini, char **cmd, t_instructions *instruc)
{
	int	fdin;
	int	fdout;

	fdin = STDIN_FILENO;
	fdout = STDOUT_FILENO;
	if (instruc->fdin.name)
		fdin = open_agreg_file(instruc->fdin.name, instruc->fdin.method);
	if (fdin == -1)
	{
		mini->last_return = 1;
		return ;
	}
	if (instruc->fdout.name)
		fdout = open_agreg_file(instruc->fdout.name, instruc->fdout.method);
	redirect(fdin, STDIN_FILENO);
	redirect(fdout, STDOUT_FILENO);
	exec_cmd(mini, cmd);
	if (dup2(mini->stdin_copy, STDIN_FILENO) == -1 ||
			dup2(mini->stdout_copy, STDOUT_FILENO) == -1)
	{
		print_errors("dup", strerror(errno), NULL, 1);
		exit_minishell(NULL, mini);
	}
}
