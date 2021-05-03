/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/13 17:46:38 by acastelb          #+#    #+#             */
/*   Updated: 2020/11/13 17:59:25 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	if (!del || !lst || !*lst)
		return ;
	if ((*lst)->next != NULL)
		ft_lstclear(&((*lst)->next), del);
	ft_lstdelone((*lst), del);
	*lst = NULL;
}
