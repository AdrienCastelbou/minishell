/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_real_input.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 14:42:04 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/12 14:48:01 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char			*update_input_with_big_quotes(char **s,
		char *new, int *i, t_mini *mini)
{
	char	*str;

	new = join_input_parts(*s, new, *i);
	*s = *s + *i + 1;
	str = *s;
	*i = 0;
	while (str[*i] != '\"' && str[*i])
	{
		if (str[*i] == '\\' && (str[*i + 1] == '$'
					|| str[*i + 1] == '\"' || str[*i + 1] == '\\'))
			new = update_input_with_echap(&str, new, i);
		else if (str[*i] == '$' &&
				str[*i + 1] && ft_isenvchar(str[*i + 1]))
			new = update_input_with_var(&str, new, i, mini->env);
		else if (str[*i] == '$' && str[*i + 1] == '?')
			new = update_input_with_last_return(&str, new, i, mini);
		else
			*i += 1;
	}
	new = join_input_parts(str, new, *i);
	*s = str + *i;
	if (**s)
		*s += 1;
	*i = 0;
	return (new);
}

char			*update_input_with_lil_quotes(char **s, char *new, int *i)
{
	char	*str;

	new = join_input_parts(*s, new, *i);
	*s = *s + *i + 1;
	str = *s;
	*i = 0;
	while (str[*i] != '\'' && str[*i])
		*i += 1;
	new = join_input_parts(*s, new, *i);
	*s = *s + *i;
	if (**s)
		*s += 1;
	*i = 0;
	return (new);
}

char			*update_input_with_echap(char **s, char *new, int *i)
{
	char	buff[2];
	char	*str;

	str = *s;
	*buff = str[*i + 1];
	buff[1] = 0;
	new = join_input_parts(*s, new, *i);
	new = join_input_parts(buff, new, 1);
	*s = *s + *i;
	if (**s)
		*s += 1;
	if (**s)
		*s += 1;
	*i = 0;
	return (new);
}

char			*update_input_with_last_return(char **s,
		char *new, int *i, t_mini *mini)
{
	char	*last_return;
	char	*tmp;

	new = join_input_parts(*s, new, *i);
	last_return = ft_itoa(mini->last_return);
	tmp = new;
	new = ft_strjoin(tmp, last_return);
	free(tmp);
	free(last_return);
	*s = *s + *i + 2;
	*i = 0;
	return (new);
}

char			*get_real_input(char *s, t_mini *mini, t_list *env)
{
	int		i;
	char	*new;
	char	*str;

	str = s;
	new = ft_strdup("");
	i = 0;
	while (s[i] && s[i] != ' ' && s[i] != 9)
	{
		if (s[i] == '\\')
			new = update_input_with_echap(&s, new, &i);
		else if (s[i] == '$' && s[i + 1] && ft_isenvchar(s[i + 1]))
			new = update_input_with_var(&s, new, &i, env);
		else if (s[i] == '\"')
			new = update_input_with_big_quotes(&s, new, &i, mini);
		else if (s[i] == '\'')
			new = update_input_with_lil_quotes(&s, new, &i);
		else if (s[i] == '$' && s[i + 1] == '?')
			new = update_input_with_last_return(&s, new, &i, mini);
		else
			i++;
	}
	new = join_input_parts(s, new, i);
	free(str);
	return (new);
}
