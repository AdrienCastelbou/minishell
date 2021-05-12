/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 13:52:08 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/12 13:53:39 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int		echo_builtin(char **splited_inputs)
{
	int	i;
	int	is_flag;

	i = 0;
	is_flag = 0;
	if (splited_inputs[1] && ft_strcmp(splited_inputs[1], "-n") == 0)
		is_flag = 1;
	i += is_flag;
	while (splited_inputs[++i])
	{
		ft_putstr_fd(splited_inputs[i], 1);
		if (splited_inputs[i + 1])
			write(1, " ", 1);
	}
	if (!is_flag)
		write(1, "\n", 1);
	return (0);
}
