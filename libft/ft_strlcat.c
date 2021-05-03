/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/05 14:58:47 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/16 14:16:30 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	int i;
	int j;

	i = -1;
	while (++i < (int)dstsize && *dst)
		dst++;
	if (i == (int)dstsize)
		return (i + ft_strlen(src));
	j = -1;
	while (src[++j])
		if (j < (int)dstsize - i - 1)
			*dst++ = src[j];
	*dst = '\0';
	return (i + j);
}
