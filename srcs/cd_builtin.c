/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_builtin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 14:05:54 by acastelb          #+#    #+#             */
/*   Updated: 2021/05/13 12:06:02 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_user_dir(t_mini *mini)
{
	char	*user;
	char	*user_path;

	user = get_env_var("USER", mini->env);
	user_path = ft_strjoin_path("/home", user);
	free(user);
	return (user_path);
}

void	update_pwd_paths(t_mini *mini)
{
	char	buff[128];
	char	*pwd[2];
	char	*old_path;

	old_path = get_env_var("PWD", mini->env);
	pwd[0] = ft_strjoin("OLDPWD=", old_path);
	pwd[1] = NULL;
	export_builtin(mini, pwd, mini->env);
	free(old_path);
	free(pwd[0]);
	ft_bzero(buff, 128);
	getcwd(buff, 128);
	pwd[0] = ft_strjoin("PWD=", buff);
	pwd[1] = NULL;
	export_builtin(mini, pwd, mini->env);
	free(pwd[0]);
}

int		cd_oldpwd(t_mini *mini)
{
	char *mov;

	mov = get_env_var("OLDPWD", mini->env);
	if (!mov || !*mov)
	{
		if (mov)
			free(mov);
		print_errors("cd", "OLDPWD", "not set", 1);
		return (1);
	}
	mini->last_return = chdir(mov);
	update_pwd_paths(mini);
	free(mov);
	return (0);
}

int		cwd_exist(void)
{
	char	buff[128];

	if (getcwd(buff, 128) == NULL)
		return (0);
	return (1);
}

int		cd_builtin(t_mini *mini, char *mov, char **inputs)
{
	int		mov_usr;

	if (mov && inputs[2])
		return (print_errors("cd", "to many arguments", NULL, 1));
	mov_usr = 0;
	if (mov == NULL)
	{
		mov = get_user_dir(mini);
		mov_usr = 1;
	}
	else if (ft_strcmp(mov, "-") == 0)
		return (cd_oldpwd(mini));
	mini->last_return = chdir(mov);
	if (!mini->last_return)
		update_pwd_paths(mini);
	else if (mini->last_return == -1)
		mini->last_return =
			print_errors("cd", mov, strerror(errno), 1);
	if (mov_usr)
		free(mov);
	if (cwd_exist() == 0)
		return (print_errors("error", GETCWD_ERR, NULL, 0));
	return (mini->last_return);
}
