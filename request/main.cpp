/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/13 15:35:19 by Ma3ert            #+#    #+#             */
/*   Updated: 2022/12/15 13:53:31 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.h"

int checkMethod(std::string method, t_start &startLine)
{
	if (method.compare("GET"))
	{
		startLine.method = "GET";
		return (1);
	}
	if (method.compare("POST"))
	{
		startLine.method = "POST";
		return (1);
	}
	if (method.compare("DELETE"))
	{
		startLine.method = "DELETE";
		return (1);
	}
	return (0);
}

int	treatAbsoluteURI(std::string target, t_start &line)
{
	if (target.compare(0, 6, "http//") || target.compare(0, 6, "HTTP//"))
	{
		target.erase(0, 6);
		size_t pos = target.find('/', 0);
		line.hostName = target.substr(0, pos);
		target.erase(0, pos);
		line.requestTraget = target.substr(0, std::string::npos);
		return (1);
	}
	return (0);
}

int treatAbsolutePath(std::string target, t_start &line)
{
	line.requestTraget = target;
}

int checkRequestTarget(std::string target, t_start &line)
{
	if (treatAbsoluteURI(target, line))
		return (1);
	if (treatAbsolutePath(target, line))
		return (1);
	return (0);
}

int	treatFirstLine(std::string file, t_start &startLine)
{
	std::string method = strtok(&file[0], " ");
	std::string version = strtok(&file[0], " ");
	std::string	target = strtok(&file[0], " ");

	if (method.empty() || version.empty() || target.empty())
		return (0);
	if (!checkMethod(method, startLine))
		return (0);
	if (!checkRequestTarget(target, startLine))
		return (0);
}

int main(int ac, char **av)
{
	std::ifstream fileRequest("request.txt");
	std::string line;
	std::string firstLine;
	t_request	newRequest;

	if (!fileRequest.is_open())
		return 1;
	getline(fileRequest, firstLine);
	if (!treatFirstLine(firstLine, newRequest.startLine))
		return 1;
	
	// while (line.empty())
	// {
	// 	getline(fileRequest, line);
	// 	treatHeaderField();
	// }
	// if (line.c_str() && newRequest.startLine.method == "POST")
	// {
	// 	std::ofstream body;
	// 	getline(fileRequest, line);
	// 	if (line.c_str())
	// 		body << line;
	// }
}