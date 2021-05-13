/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 10:08:08 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/13 17:52:45 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	get_cmd_token(t_mini *mini, t_list **cmd, char *s, int len)
{
	t_list	*new;
	char	*token;

	token = get_real_input(ft_strndup(s, len), mini, mini->env);
	new = ft_lstnew(token);
	ft_lstadd_back(cmd, new);
}

t_list	*ft_lst_input(t_mini *mini, t_instructions *instruc, char *s)
{
	t_list	*cmd;
	int		len;

	if (!s)
		return (NULL);
	cmd = NULL;
	while (*s)
	{
		while ((*s == ' ' || *s == 9) && *s)
			s++;
		if (!(*s))
			return (cmd);
		len = ft_word_size(s);
		if ((*s == '>' || *s == '<'))
		{
			if (parse_fd(mini, instruc, s, len))
				return (cmd);
		}
		else
			get_cmd_token(mini, &cmd, s, len);
		s += len;
	}
	if (instruc->is_empty == 0 && cmd == NULL)
		cmd = ft_lstnew(ft_strdup(""));
	return (cmd);
}

int		get_instructions(t_mini *mini, char *s, int len, t_list *cmd)
{
	char			*instruction;
	t_instructions	*current;

	mini->instructions = NULL;
	if (*s == '|')
		return (parsing_error('|'));
	while (*(s += len))
	{
		if (*s == '|')
			s += 1;
		current = ft_instructnew(NULL);
		len = ft_cmd_size(s, '|');
		if (s[len] == '|')
			mini->is_pipe = 1;
		if (!(instruction = ft_strndup(s, len)))
			return (print_errors("malloc", strerror(errno), NULL, 1));
		cmd = ft_lst_input(mini, current, instruction);
		current->cmds = cmd;
		ft_instruct_add_back(&mini->instructions, current);
		free(instruction);
		if (mini->is_pipe && current->is_empty &&
				(!cmd || !*((char *)cmd->content)))
			return (parsing_error('|'));
	}
	return (0);
}

void	parse_and_run(t_mini *mini, char *s, int cmd_nb)
{
	char	*cmd_input;
	int		i;
	int		len;

	i = -1;
	while (++i < cmd_nb)
	{
		len = ft_cmd_size(s, ';');
		cmd_input = ft_strndup(s, len);
		mini->last_return = get_instructions(mini, cmd_input, 0, NULL);
		if (mini->last_return || mini->is_fd_err)
		{
			free_current_cmd(mini, cmd_input);
			return ;
		}
		if (mini->is_pipe)
			make_pipe(mini, mini->instructions);
		else if (mini->instructions)
		{
			mini->cmd = get_cmd_tab(mini->instructions->cmds);
			run_cmd(mini, mini->cmd, mini->instructions);
		}
		free_current_cmd(mini, cmd_input);
		s += len + 1;
	}
}

t_list	*ft_lst_cmds(t_mini *mini, char *s)
{
	int		cmd_nb;

	if (!s)
		return (NULL);
	cmd_nb = cmd_count(s);
	if (cmd_nb < 1)
	{
		if (!cmd_nb)
			mini->last_return = parsing_error(';');
		else
			mini->last_return = 258;
		free_current_cmd(mini, NULL);
		return (NULL);
	}
	parse_and_run(mini, s, cmd_nb);
	return (mini->cmds);
}
