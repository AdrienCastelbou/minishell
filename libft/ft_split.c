/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/16 10:41:13 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/20 15:53:14 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int		ft_word_size(const char *s, char c)
{
	int		len;

	len = 0;
	while (s[len] != c && s[len])
		len++;
	return (len);
}

static int		ft_words_count(char const *s, char c)
{
	int		count;
	int		len;

	count = 0;
	while (*s)
	{
		while (*s == c)
			s++;
		if ((len = ft_word_size(s, c)))
		{
			s += len;
			count++;
		}
	}
	return (count);
}

static char		*ft_create_w(const char *s, int len)
{
	char	*str;

	if (!(str = (char *)malloc(sizeof(char) * (len + 1))))
		return (NULL);
	str[len] = '\0';
	while (len--)
		str[len] = s[len];
	return (str);
}

static void		ft_free_tab(char **splited, int size)
{
	int i;

	i = -1;
	while (++i < size)
		free(splited[i]);
	free(splited);
}

char			**ft_split(char const *s, char c)
{
	int		words_nb;
	char	**splited;
	int		i;
	int		len;

	if (!s)
		return (NULL);
	words_nb = ft_words_count(s, c);
	if (!(splited = (char **)malloc(sizeof(char *) * (words_nb + 1))))
		return (NULL);
	i = -1;
	while (++i < words_nb)
	{
		while (*s == c && *s)
			s++;
		len = ft_word_size(s, c);
		if (!(splited[i] = ft_create_w(s, len)))
		{
			ft_free_tab(splited, i);
			return (NULL);
		}
		s += len;
	}
	splited[i] = NULL;
	return (splited);
}
