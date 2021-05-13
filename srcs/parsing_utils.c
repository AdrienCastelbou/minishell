/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 10:20:46 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/13 10:22:52 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		ft_get_fd_token(const char *s)
{
	int	len;

	len = 0;
	if (s[len] == '<')
		len += 1;
	else if (s[len] == '>' && s[len + 1] != '>')
		len += 1;
	else
		len += 2;
	while (s[len] && (s[len] == ' ' || s[len] == 9))
		len++;
	len += ft_word_size(s + len);
	return (len);
}

int		ft_cmd_size(const char *s, char c)
{
	int		len;

	len = 0;
	while (s[len])
	{
		if (s[len] == c)
			return (len);
		else if (s[len] == '\"' || s[len] == '\'')
			jump_quotes_in_parsing((char *)s, s[len], &len);
		else if (s[len] == '\\')
			len += 1;
		if (!s[len])
			return (len);
		len++;
	}
	return (len);
}

int		parse_fd(t_mini *mini, t_instructions *instruc, char *s, int len)
{
	if (*s == '>')
	{
		instruc->is_empty = 0;
		return (get_fdout_file(instruc, ft_strndup(s, len), mini));
	}
	else if (*s == '<')
	{
		instruc->is_empty = 0;
		return ((get_fdin_file(instruc, ft_strndup(s, len), mini)));
	}
	return (0);
}
