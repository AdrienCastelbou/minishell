/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/05 15:06:07 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/16 16:48:16 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_trim_start(const char *s, const char *set)
{
	int		i;
	int		trim_count;

	i = -1;
	trim_count = 0;
	while (ft_strchr(set, s[++i]))
		trim_count += 1;
	return (trim_count);
}

static int	ft_trim_end(const char *s, int len, const char *set)
{
	int		i;
	int		trim_count;

	i = 0;
	trim_count = 0;
	while (++i && ft_strchr(set, s[len - i]))
		trim_count += 1;
	return (trim_count);
}

char		*ft_strtrim(char const *s, char const *set)
{
	char	*str;
	int		len;
	int		i;

	if (!s)
		return (NULL);
	len = ft_strlen(s);
	len = len - ft_trim_start(s, set) - ft_trim_end(s, len, set);
	if (len < 0)
		len = 0;
	if (!(str = (char *)malloc(sizeof(char) * (len + 1))))
		return (NULL);
	while (ft_strchr(set, *s))
		s++;
	i = -1;
	while (++i < len)
		str[i] = s[i];
	str[i] = '\0';
	return (str);
}
