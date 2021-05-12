/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   historic.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 11:00:23 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/12 11:09:00 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	erase_current_line(t_mini *mini, t_cursor *curs)
{
	tputs(tgoto(mini->cm_cap, 0, curs->cur_line - 1), 1, ft_putchar);
	tputs(mini->ce_cap, 1, ft_putchar);
	curs->cur_col = mini->ws.ws_col - 1;
	curs->cur_line -= 1;
	tputs(tgoto(mini->cm_cap, curs->cur_col,
				curs->cur_line - 1), 1, ft_putchar);
}

void	erase_current_prompt(t_mini *mini, int *top, char *buff, t_cursor *curs)
{
	int		len;
	int		i;
	int		isnt_endline;

	ioctl(STDIN_FILENO, TIOCGWINSZ, &mini->ws);
	isnt_endline = 1;
	get_cursor_position(&curs->cur_col, &curs->cur_line);
	len = ft_strlen(mini->history->input);
	i = -1;
	while (++i < len)
	{
		if (curs->cur_col == mini->ws.ws_col)
			isnt_endline = 0;
		if (curs->cur_col > 0)
			curs->cur_col -= 1;
		else
			erase_current_line(mini, curs);
	}
	tputs(tgoto(mini->cm_cap, curs->cur_col - isnt_endline,
				curs->cur_line - 1), 1, ft_putchar);
	tputs(mini->ce_cap, 1, ft_putchar);
	ft_bzero(mini->history->input, ft_strlen(mini->history->input));
	ft_bzero(buff, ft_strlen(buff));
	*top = 0;
}

int		add_input_in_history(t_mini *mini)
{
	t_history	*elem;

	if (mini->history && !*mini->history->input)
	{
		mini->current_hist = mini->history;
		return (1);
	}
	elem = ft_historynew();
	if (!elem)
		return (0);
	if (mini->history)
		ft_history_add_front(&mini->history, elem);
	else
		mini->history = elem;
	mini->current_hist = mini->history;
	return (1);
}

int		up_history(t_mini *mini, int *top, char *buff, t_cursor *cursor)
{
	int		len;
	int		i;
	char	*new;

	i = -1;
	if (!mini->current_hist->next)
		return (1);
	if (!(join_prompt_parts(mini, buff)))
		return (-1);
	erase_current_prompt(mini, top, buff, cursor);
	mini->current_hist = mini->current_hist->next;
	new = mini->current_hist->input;
	len = ft_strlen(new);
	while (++i < len)
		if (write_char_in_prompt(mini, new[i], top, buff) == 0)
			return (-1);
	return (1);
}

int		down_history(t_mini *mini, int *top, char *buff, t_cursor *cursor)
{
	int		len;
	int		i;
	char	*new;

	i = -1;
	if (!mini->current_hist->previous)
		return (1);
	if (!(join_prompt_parts(mini, buff)))
		return (-1);
	erase_current_prompt(mini, top, buff, cursor);
	mini->current_hist = mini->current_hist->previous;
	new = mini->current_hist->input;
	len = ft_strlen(new);
	while (++i < len)
		if (write_char_in_prompt(mini, new[i], top, buff) == 0)
			return (-1);
	return (1);
}
