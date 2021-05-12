/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   instructions.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 12:07:17 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/12 12:11:22 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_instructions	*ft_instructnew(t_list *content)
{
	t_instructions	*elem;

	if (!(elem = (t_instructions*)malloc(sizeof(t_instructions))))
		return (NULL);
	elem->cmds = content;
	elem->fdin.fd = 0;
	elem->fdin.name = NULL;
	elem->fdin.is_file = 0;
	elem->is_empty = 1;
	elem->fdout.fd = 1;
	elem->fdout.is_file = 0;
	elem->fdout.name = NULL;
	elem->next = NULL;
	return (elem);
}

void			ft_instruct_add_back(t_instructions **alst, t_instructions *new)
{
	t_instructions *elem;

	if (!alst)
		return ;
	if (!*alst)
	{
		*alst = new;
		return ;
	}
	elem = *alst;
	while (elem->next)
		elem = elem->next;
	elem->next = new;
}

void			ft_instrucdelone(t_instructions *instruc)
{
	if (!instruc)
		return ;
	ft_lstclear(&(instruc->cmds), free);
	if (instruc->fdin.name)
		free(instruc->fdin.name);
	instruc->fdin.name = NULL;
	if (instruc->fdout.name)
		free(instruc->fdout.name);
	instruc->fdout.name = NULL;
	free(instruc);
}

void			ft_instruclear(t_instructions **instruc)
{
	if (!instruc || !*instruc)
		return ;
	if ((*instruc)->next != NULL)
		ft_instruclear(&((*instruc)->next));
	ft_instrucdelone((*instruc));
	*instruc = NULL;
}
