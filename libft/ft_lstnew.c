/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/05 14:37:33 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/16 14:40:45 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*elem;

	if (!(elem = (t_list*)malloc(sizeof(t_list))))
		return (NULL);
	elem->content = (void *)content;
	elem->next = NULL;
	return (elem);
}
