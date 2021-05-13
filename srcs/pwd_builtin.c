/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 14:37:38 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/13 12:25:58 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		pwd_builtin(void)
{
	char	buff[128];
	char	*buff_copy;
	int		status;

	status = 0;
	ft_bzero(buff, 128);
	if ((buff_copy = getcwd(buff, 128)) != NULL)
	{
		ft_putstr_fd(buff_copy, STDOUT_FILENO);
		ft_putchar_fd('\n', STDOUT_FILENO);
	}
	else
	{
		print_errors("pwd", GETCWD_ERR, NULL, 0);
		status = 1;
	}
	return (status);
}
