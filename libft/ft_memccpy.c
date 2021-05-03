/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memccpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/05 14:38:50 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/03 15:05:46 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memccpy(void *dst, const void *src, int c, size_t n)
{
	int		i;
	char	*s_dst;
	char	*s_src;

	i = -1;
	s_dst = (char *)dst;
	s_src = (char *)src;
	while (++i < (int)n)
	{
		s_dst[i] = s_src[i];
		if (s_src[i] == (char)c)
			return (dst + i + 1);
	}
	return (NULL);
}
