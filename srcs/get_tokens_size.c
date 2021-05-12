/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_tokens_size.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 14:55:31 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/12 14:57:47 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		ft_word_size(const char *s)
{
	int		len;

	len = 0;
	if ((s[len] == '>' || s[len] == '<'))
		return (ft_get_fd_token(s));
	while (s[len])
	{
		if (s[len] == ' ' || s[len] == 9 ||
				s[len] == '>' || s[len] == '<')
			return (len);
		else if (s[len] == '\"' || s[len] == '\'')
			jump_quotes_in_parsing((char *)s, s[len], &len);
		else if (s[len] == '\\')
			len += 1;
		if (s[len])
			len++;
	}
	return (len);
}

int		check_cmd_end(char c, int *isnt_blank, int *count)
{
	if (c != ' ' && c != 9 && c != ';')
		*isnt_blank = 1;
	if (c == ';')
	{
		if (!*isnt_blank)
			return (0);
		*count += 1;
		*isnt_blank = 0;
	}
	return (1);
}

int		jump_quotes_in_parsing(char *input, char quote, int *i)
{
	while (input[++(*i)])
	{
		if (input[*i] == '\\')
			*i += 1;
		else if (input[*i] == quote)
			break ;
	}
	return (1);
}

int		detect_quotes_in_input(char *input, char quote, int *i)
{
	jump_quotes_in_parsing(input, quote, i);
	if (!input[*i])
		return (quote_error_in_parsing(quote));
	return (1);
}

int		cmd_count(char *input)
{
	int		i;
	int		count;
	int		isnt_blank;

	isnt_blank = 0;
	i = 0;
	count = 1;
	while (input[i])
	{
		if (!check_cmd_end(input[i], &isnt_blank, &count))
			return (0);
		else if (input[i] == '\"' || input[i] == '\'')
		{
			if (detect_quotes_in_input(input, input[i], &i) == -1)
				return (-1);
		}
		else if (input[i] == '\\')
			i++;
		if (!input[i])
			return (count);
		i++;
	}
	return (count);
}
