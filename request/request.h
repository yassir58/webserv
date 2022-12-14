/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 16:01:13 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/10 19:26:45 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <fstream>
# include <list>
# include <iostream>
# include <cstring>
# include <vector>

typedef std::vector<std::string> stringContainer;

typedef struct start
{
	std::string method;
	std::string httpVersion;
	std::string	requestTarget;
	std::string	hostName;
	std::string IpAdress;
	std::string Port;
	bool		Host;
} t_start;

typedef struct headerField
{
	std::string key;
	std::string value;
} headerField;

typedef std::list<headerField>	headerFieldList;

typedef struct request
{
	t_start		startLine;
	headerFieldList	generalField;
	headerFieldList	requestField;
	headerFieldList	entityField;
} t_request;

