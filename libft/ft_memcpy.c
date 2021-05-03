/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/05 14:41:24 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/13 17:12:31 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t	i;
	char	*t_dst;
	char	*t_src;

	if (dst == src)
		return (dst);
	t_dst = (char *)dst;
	t_src = (char *)src;
	i = 0;
	while (i < n)
	{
		t_dst[i] = t_src[i];
		i++;
	}
	return (dst);
}
