/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 15:31:52 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/12 15:33:18 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		print_errors(char *cmd, char *error, char *more, int nb)
{
	ft_putstr_fd("\U0000274C minishell: ", STDERR_FILENO);
	if (cmd && *cmd)
	{
		ft_putstr_fd(cmd, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	if (cmd && (!ft_strcmp("export", cmd) || !ft_strcmp("unset", cmd)))
		ft_putchar_fd('\'', STDERR_FILENO);
	if (error && *error)
		ft_putstr_fd(error, STDERR_FILENO);
	if (cmd && (!ft_strcmp("export", cmd) || !ft_strcmp("unset", cmd)))
		ft_putchar_fd('\'', STDERR_FILENO);
	if (more && *more)
	{
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(more, STDERR_FILENO);
	}
	ft_putchar_fd('\n', STDERR_FILENO);
	return (nb);
}

int		quote_error_in_parsing(char c)
{
	ft_putstr_fd("\U0000274C minishell: quote ", STDERR_FILENO);
	ft_putchar_fd(c, STDERR_FILENO);
	ft_putstr_fd(" is not closed\n", STDERR_FILENO);
	return (-1);
}

int		parsing_error(char c)
{
	ft_putstr_fd("\U0000274C minishell: ", STDERR_FILENO);
	ft_putstr_fd("syntax error near unexpected token", STDERR_FILENO);
	if (c == '|')
		ft_putstr_fd(" \'|\'\n", STDERR_FILENO);
	else if (c == '>')
		ft_putstr_fd(" \'newline\'\n", STDERR_FILENO);
	else if (c == ';')
		ft_putstr_fd(" \';\'\n", STDERR_FILENO);
	return (258);
}
