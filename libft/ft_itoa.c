/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/05 14:32:37 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/05 14:34:29 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

static int	get_size(long int n)
{
	int			i;

	i = 1;
	if (n < 0)
	{
		n *= -1;
		i++;
	}
	while (n >= 10)
	{
		n /= 10;
		i++;
	}
	return (i);
}

static char	*create_str(char *str, long int n, int len)
{
	long int	nbr;

	str[len] = '\0';
	nbr = (n < 0) ? n * -1 : n;
	while (len--)
	{
		str[len] = nbr % 10 + '0';
		nbr /= 10;
	}
	if (n < 0)
		str[0] = '-';
	return (str);
}

char		*ft_itoa(int n)
{
	char		*str;
	int			len;

	len = get_size(n);
	if (!(str = (char *)malloc(sizeof(char) * (len + 1))))
		return (NULL);
	str = create_str(str, n, len);
	return (str);
}
