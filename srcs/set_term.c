/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_term.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 11:49:50 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/14 09:16:15 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	sig_handler(int signum)
{
	if (signum == SIGINT)
	{
		g_sig_catcher.should_run = 0;
		close(STDIN_FILENO);
	}
	if (signum == SIGQUIT && g_sig_catcher.pid > 0)
	{
		g_sig_catcher.should_run = -1;
		kill(g_sig_catcher.pid, SIGQUIT);
	}
}

t_list	*copy_env(char **envp)
{
	t_list	*env;
	t_list	*elem;
	char	*env_var;

	env = NULL;
	elem = NULL;
	while (*envp)
	{
		if (ft_strnstr(*envp, "SHLVL=", 6) == *envp)
			env_var = get_shlvl(*envp);
		else
			env_var = ft_strdup(*envp);
		if (!env_var || !(elem = ft_lstnew(env_var)))
		{
			if (!env_var)
				free(env_var);
			ft_lstclear(&env, free);
			return (NULL);
		}
		ft_lstadd_back(&env, elem);
		envp++;
	}
	return (env);
}

char	*get_shlvl(char *shlvl)
{
	char	*value;
	int		inted_value;
	char	*env_var;

	while (!ft_isdigit(*shlvl))
		shlvl++;
	inted_value = ft_atoi(shlvl) + 1;
	if (!(value = ft_itoa(inted_value)))
		return (NULL);
	env_var = ft_strjoin("SHLVL=", value);
	free(value);
	return (env_var);
}

int		set_mode(void)
{
	struct termios	t;
	int				r;

	tcgetattr(STDIN_FILENO, &g_sig_catcher.saved_attributes);
	r = tcgetattr(STDIN_FILENO, &t);
	if (r)
		return (0);
	t.c_lflag &= ~(ICANON | ECHO);
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;
	r = tcsetattr(STDIN_FILENO, TCSANOW, &t);
	if (r)
		return (0);
	return (1);
}

int		reset_input_mode(void)
{
	int	r;

	r = tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_sig_catcher.saved_attributes);
	if (r)
		return (0);
	return (1);
}
