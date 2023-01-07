/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 16:01:13 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/06 17:46:52 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <fstream>
#include <list>
#include <iostream>
#include <cstring>
#include <vector>

typedef struct start
{
	std::string method;
	std::string httpVersion;
	std::string	requestTarget;
	std::string	hostName;
} t_start;

typedef struct headerField
{
	std::string key;
	std::string value;
} headerField;

typedef struct request
{
	t_start		startLine;
	std::list<headerField>	generalField;
	std::list<headerField>	requestField;
	std::list<headerField>	entityField;
} t_request;

