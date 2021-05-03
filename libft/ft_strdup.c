/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/05 14:55:32 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/16 12:21:43 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *src)
{
	char	*dest;
	int		i;

	if (!(dest = (char *)malloc(sizeof(char) * (ft_strlen(src) + 1))))
		return (NULL);
	i = -1;
	while (src[++i])
		dest[i] = src[i];
	dest[i] = src[i];
	return (dest);
}
