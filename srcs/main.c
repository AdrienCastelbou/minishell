/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 10:36:18 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/13 18:49:19 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		interrupt_prompt(t_mini *mini)
{
	mini->last_return = 1;
	if (dup2(mini->stdin_copy, STDIN_FILENO) == -1)
	{
		print_errors("dup", strerror(errno), NULL, 1);
		exit_minishell(NULL, mini);
	}
	reset_input_mode();
	if (mini->input && *(mini->input))
		ft_bzero(mini->input, ft_strlen(mini->input));
	if (mini->history->input && *(mini->history->input))
		ft_bzero(mini->history->input, ft_strlen(mini->history->input));
	return (1);
}

int		ft_get_input(t_mini *mini)
{
	int	ret;

	g_sig_catcher.should_run = 1;
	g_sig_catcher.pid = -1;
	if (!mini->last_return)
		ft_putstr_fd("\U0001F49A ", STDOUT_FILENO);
	else
		ft_putstr_fd("\U0001F494 ", STDOUT_FILENO);
	ft_putstr_fd("\033[0;34mminishell> \033[0m", STDOUT_FILENO);
	if (g_sig_catcher.should_run == 0)
		return (interrupt_prompt(mini));
	if ((ret = read_prompt(mini)) == 0)
	{
		if (g_sig_catcher.should_run == 0)
			return (interrupt_prompt(mini));
		print_errors("prompt", strerror(errno), NULL, 1);
		exit_minishell(NULL, mini);
	}
	if (ft_strchr(mini->input, '\n'))
		*(ft_strchr(mini->input, '\n')) = '\0';
	ft_lst_cmds(mini, mini->input);
	free(mini->input);
	mini->input = NULL;
	return (1);
}

int		main(int argc, char **argv, char **envp)
{
	t_mini	*mini;
	int		ret;
	char	*term_type;
	t_list	*env;

	(void)argc;
	(void)argv;
	if (!envp || !*envp)
		env = set_basic_env();
	else
		env = copy_env(envp);
	if (!env)
		return (print_errors("malloc", strerror(errno), NULL, 1));
	term_type = get_env_var("TERM", env);
	ret = tgetent(NULL, term_type);
	if (!term_type || ret < 1)
		return (termcaps_error(env, term_type));
	free(term_type);
	if (!(mini = init_mini(env)))
		return (print_errors("init minishell", strerror(errno), NULL, 1));
	signal(SIGINT, sig_handler);
	signal(SIGQUIT, sig_handler);
	while (1)
		ft_get_input(mini);
}
