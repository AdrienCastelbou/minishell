/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fds.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 13:35:53 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/13 17:49:01 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		open_agreg_file(char *file, char *method)
{
	int fd;

	if (strcmp(method, ">") == 0)
		fd = open(file, O_WRONLY | O_TRUNC |
				O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
	else if (strcmp(method, "<") == 0)
		fd = open(file, O_RDONLY);
	else
		fd = open(file, O_WRONLY | O_APPEND |
				O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
	return (fd);
}

int		create_and_close_file(char *file, char *method)
{
	int fd;

	if (strcmp(method, ">") == 0)
		fd = open(file, O_WRONLY | O_TRUNC |
				O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
	else if (strcmp(method, "<") == 0)
		fd = open(file, O_RDONLY);
	else
		fd = open(file, O_WRONLY | O_APPEND |
				O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
	if (fd < 0)
		return (print_errors(file, strerror(errno), NULL, 1));
	close(fd);
	return (0);
}

char	*get_open_method(char *s, int *i)
{
	char	*method;

	if (s[*i] == '>' && s[*i + 1] != '>')
	{
		*i += 1;
		method = ">";
	}
	else if (s[*i] == '>' && s[*i + 1] == '>')
	{
		*i += 2;
		method = ">>";
	}
	while (s[*i] && (s[*i] == ' ' || s[*i] == 9))
		*i += 1;
	return (method);
}

int		get_fdout_file(t_instructions *instruct, char *s, t_mini *mini)
{
	char	*method;
	char	*file;
	int		i;
	int		size;

	i = 0;
	method = get_open_method(s, &i);
	if (s[i] == 0)
	{
		mini->is_fd_err = 1;
		free(s);
		return (parsing_error('>'));
	}
	size = ft_word_size(s + i);
	if (instruct->fdout.name)
	{
		free(instruct->fdout.name);
		instruct->fdout.name = NULL;
	}
	file = get_real_input(ft_strndup(s + i, size), mini, mini->env);
	instruct->fdout.name = file;
	instruct->fdout.method = method;
	instruct->fdout.is_file = 1;
	create_and_close_file(file, method);
	free(s);
	return (0);
}

int		get_fdin_file(t_instructions *instruct, char *s, t_mini *mini)
{
	char	*method;
	char	*file;
	int		i;

	method = "<";
	i = 1;
	while (s[i] && (s[i] == ' ' || s[i] == 9))
		i++;
	if (s[i] == 0)
	{
		mini->is_fd_err = 1;
		free(s);
		return (parsing_error('>'));
	}
	if (instruct->fdin.name)
	{
		free(instruct->fdin.name);
		instruct->fdin.name = NULL;
	}
	file = get_real_input(ft_strndup(s + i,
				ft_word_size(s + i)), mini, mini->env);
	instruct->fdin.name = file;
	instruct->fdin.method = method;
	instruct->fdin.is_file = 1;
	free(s);
	return (create_and_close_file(file, method));
}
