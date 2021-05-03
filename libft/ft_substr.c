/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/14 12:05:08 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/18 14:07:30 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*str;
	int		i;

	i = 0;
	if (!(str = (char *)malloc(sizeof(char) * (len + 1))) || !s)
		return (NULL);
	if ((int)start < ft_strlen(s))
		while (i < (int)len && s[i + start])
		{
			str[i] = s[start + i];
			i++;
		}
	str[i] = '\0';
	return (str);
}
