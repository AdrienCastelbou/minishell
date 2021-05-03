/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/13 16:28:03 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/18 14:06:39 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strlcpy(char *dst, const char *src, size_t dstsize)
{
	int	i;

	if (!dst || !src)
		return (0);
	i = -1;
	if (dstsize)
	{
		while (++i < (int)dstsize - 1 && src[i])
			dst[i] = src[i];
		dst[i] = '\0';
	}
	return (ft_strlen(src));
}
