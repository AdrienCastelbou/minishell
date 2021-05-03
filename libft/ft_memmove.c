/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/05 14:42:28 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/16 12:01:28 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	if (dst > src)
		while (len)
		{
			((char *)dst)[len - 1] = ((char *)src)[len - 1];
			len--;
		}
	else
		ft_memcpy(dst, src, len);
	return (dst);
}
