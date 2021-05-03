/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/05 14:45:58 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/19 12:35:28 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void		ft_display(long int nb, int fd)
{
	if (nb >= 10)
		ft_display(nb / 10, fd);
	ft_putchar_fd(nb % 10 + '0', fd);
}

void			ft_putnbr_fd(int n, int fd)
{
	long int nb;

	nb = n;
	if (nb < 0)
	{
		ft_putchar_fd('-', fd);
		nb *= -1;
	}
	ft_display(nb, fd);
}
