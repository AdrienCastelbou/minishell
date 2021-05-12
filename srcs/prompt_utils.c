/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 11:41:12 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/12 11:48:23 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		erase_char_in_prompt(t_mini *mini, int *top, char *buff)
{
	ioctl(STDIN_FILENO, TIOCGWINSZ, &mini->ws);
	if (!(*mini->history->input) && !*buff)
		return (1);
	if (!*top && *mini->history->input)
		mini->history->input[ft_strlen(mini->history->input) - 1] = 0;
	else if (*top)
	{
		*top -= 1;
		buff[*top] = 0;
	}
	get_cursor_position(&(mini->cursor.cur_col), &(mini->cursor.cur_line));
	if (mini->cursor.cur_col > 1)
		tputs(mini->le_cap, 1, ft_putchar);
	else
		tputs(tgoto(mini->cm_cap, mini->ws.ws_col - 1,
					mini->cursor.cur_line - 2), 1, ft_putchar);
	tputs(mini->dc_cap, 1, ft_putchar);
	return (1);
}

int		write_char_in_prompt(t_mini *mini, char c, int *top, char *buff)
{
	buff[*top] = c;
	write(1, &c, 1);
	*top += 1;
	if (*top >= 128 - 1)
	{
		if (join_prompt_parts(mini, buff) == 0)
			return (-1);
		*top = 0;
	}
	return (1);
}

int		is_arrow(char *buff)
{
	if (buff[0] == 27 && buff[1] == 91 && buff[2] == 65)
		return (1);
	else if (buff[0] == 27 && buff[1] == 91 && buff[2] == 66)
		return (1);
	return (0);
}

int		ft_putchar(int c)
{
	write(1, &c, 1);
	return (1);
}
