/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 16:01:13 by Ma3ert            #+#    #+#             */
/*   Updated: 2022/12/14 16:32:37 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <fstream>
#include <iostream>
#include <cstring>

typedef struct start
{
	std::string method;
	std::string httpVersion;
	std::string	requestTraget;
} t_start;

typedef struct request
{
	t_start	startLine; 
}t_request;

