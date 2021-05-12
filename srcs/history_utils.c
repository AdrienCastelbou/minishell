/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 15:34:49 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/12 15:36:26 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void		free_history(t_history **elem)
{
	if (!elem || !*elem)
		return ;
	if ((*elem)->next)
		free_history(&(*elem)->next);
	if ((*elem)->input)
	{
		free((*elem)->input);
		(*elem)->input = NULL;
	}
	free((*elem));
	*elem = NULL;
}

t_history	*ft_historynew(void)
{
	t_history	*elem;

	if (!(elem = malloc(sizeof(t_history) * 1)))
		return (NULL);
	elem->input = malloc(sizeof(char) * 1);
	*elem->input = 0;
	elem->is_prompt = 1;
	elem->previous = NULL;
	elem->next = NULL;
	return (elem);
}

void		ft_history_add_front(t_history **ahist, t_history *new)
{
	if (!ahist)
		return ;
	(*ahist)->previous = new;
	new->next = *ahist;
	*ahist = new;
}
