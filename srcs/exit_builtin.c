/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 13:56:27 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/13 11:12:04 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

long long int	ft_atolli(const char *str)
{
	int				i;
	int				is_neg;
	long long int	result;

	i = 0;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	is_neg = 1;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			is_neg *= -1;
		i++;
	}
	result = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * is_neg);
}

int				is_only_digit(char *s)
{
	int i;

	i = 0;
	if (!ft_isdigit(s[i]) && s[i] != '-' && s[i] != '+')
		return (0);
	while (s[++i])
		if (!ft_isdigit(s[i]))
			return (0);
	return (1);
}

int				is_bad_num_value(char *s, long long int nb)
{
	if (nb > 0 && (*s == '-'))
		return (1);
	else if (nb < 0 && (*s != '-'))
		return (1);
	return (0);
}

int				exit_minishell(char **splited_inputs, t_mini *mini)
{
	long long int	return_value;

	if (!mini->is_pipe)
		ft_putstr_fd("exit\n", STDERR_FILENO);
	return_value = mini->last_return;
	if (splited_inputs)
	{
		if (splited_inputs[1] && !is_only_digit(splited_inputs[1]))
			return_value = print_errors("exit",
					splited_inputs[1], "numeric argument required", 2);
		else if (splited_inputs[1] && splited_inputs[2])
			return (print_errors("exit", "too many arguments", NULL, 1));
		else if (splited_inputs[1])
			return_value = ft_atolli(splited_inputs[1]);
		else if (is_bad_num_value(splited_inputs[1], return_value))
			return_value = print_errors("exit",
					splited_inputs[1], "numeric argument required", 255);
	}
	if (!mini->is_pipe)
		free_mini(mini);
	exit(return_value);
	return (0);
}
