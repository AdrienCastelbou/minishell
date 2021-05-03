/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/05 14:39:52 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/16 12:03:03 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	char	*t_s;
	int		i;

	t_s = (char *)s;
	i = -1;
	while (++i < (int)n)
		if ((char)c == t_s[i])
			return (t_s + i);
	return (NULL);
}
