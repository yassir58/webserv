/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   responseUtils.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 11:49:00 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/02/20 11:49:50 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

int	ft_strlen(const char *str)
{
	int i;
	while (str[i])
		i++;
	return (i);
}

char	*ft_strjoin(char *s1, char const *s2)
{
	int		i;
	int		j;
	int		l2;
	char	*str;

	i = -1;
	j = 0;
	if (!s1 || !s2)
		return (0);
	l2 = ft_strlen(s2);
	str = new char[(sizeof(char) * ft_strlen(s1)) + (sizeof(char) * l2 + 1)];
	if (!str)
		return (0);
	while (s1[++i])
		str[i] = s1[i];
	while (s2[j])
	{
		str[i] = s2[j];
		i++;
		j++;
	}
	str[i] = '\0';
	delete s1;
	return (str);
}