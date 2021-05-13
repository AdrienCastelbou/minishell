/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 11:13:15 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/13 18:50:14 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		join_prompt_parts(t_mini *mini, char *buff)
{
	char *tmp;

	tmp = mini->history->input;
	mini->history->input = ft_strjoin(tmp, buff);
	free(tmp);
	ft_bzero(buff, ft_strlen(buff));
	if (!mini->history->input)
		return (0);
	return (1);
}

void	get_cursor_position(int *col, int *line)
{
	char	buff[128];
	int		i;

	ft_bzero(buff, 128);
	ft_putstr_fd("\033[6n", STDIN_FILENO);
	read(1, buff, 127);
	i = 2;
	*line = ft_atoi(&buff[i]);
	while (buff[i] && buff[i] != ';')
		i++;
	i += 1;
	*col = ft_atoi(&buff[i]);
}

int		check_input_validity(t_mini *mini, char *buffchar, int *top, char *buff)
{
	int	i;

	i = -1;
	while (++i < 3 && buffchar[i])
	{
		if (ft_isprint(buffchar[i]))
			if (write_char_in_prompt(mini, buffchar[i], top, buff) == 0)
				return (-1);
	}
	return (1);
}

int		check_prompt_input(t_mini *mini, int *top, char *buffchar, char *buff)
{
	int ret;

	if (*buffchar == '\004')
	{
		if (!(*mini->history->input) && !*buff)
			exit_minishell(NULL, mini);
	}
	else if (is_arrow(buffchar) && buffchar[2] == 65)
		ret = up_history(mini, top, buff, &mini->cursor);
	else if (is_arrow(buffchar) && buffchar[2] == 66)
		ret = down_history(mini, top, buff, &mini->cursor);
	else if (*buffchar == '\n')
		return (0);
	else if (*buffchar == 127)
		ret = erase_char_in_prompt(mini, top, buff);
	else if (ft_isprint(*buffchar))
		ret = check_input_validity(mini, buffchar, top, buff);
	ft_bzero(buffchar, 3);
	return (ret);
}

int		read_prompt(t_mini *mini)
{
	char		buffchar[3];
	char		buff[128];
	int			top;
	int			ret;

	if (!add_input_in_history(mini) || !set_mode())
	{
		write(1, "\n", 1);
		return (0);
	}
	mini->current_hist = mini->history;
	ret = 0;
	top = 0;
	ft_bzero(buff, 128);
	ft_bzero(buffchar, 3);
	while (read(STDIN_FILENO, buffchar, 3) && g_sig_catcher.should_run)
		if ((ret = check_prompt_input(mini, &top, buffchar, buff)) < 1)
			break ;
	write(1, "\n", 1);
	if (ret == -1 || !reset_input_mode() || !(join_prompt_parts(mini, buff)))
		return (0);
	if (!(mini->input = ft_strdup(mini->history->input)))
		return (0);
	mini->history->is_prompt = 0;
	return (1);
}
