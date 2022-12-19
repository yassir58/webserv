/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/13 15:35:19 by Ma3ert            #+#    #+#             */
/*   Updated: 2022/12/19 18:24:38 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.h"

int checkMethod(std::string method, t_start &startLine)
{
	if (!method.compare("GET"))
	{
		startLine.method = "GET";
		return (1);
	}
	if (!method.compare("POST"))
	{
		startLine.method = "POST";
		return (1);
	}
	if (!method.compare("DELETE"))
	{
		startLine.method = "DELETE";
		return (1);
	}
	return (0);
}

int	treatAbsoluteURI(std::string target, t_start &line)
{
	if (!target.compare(0, 6, "http//") || !target.compare(0, 6, "HTTP//"))
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
	return (1);
}

int checkRequestTarget(std::string target, t_start &line)
{
	if (treatAbsoluteURI(target, line))
		return (1);
	if (treatAbsolutePath(target, line))
		return (1);
	return (0);
}

int parseFirstLine(std::string line, std::string &method, std::string &target, std::string &version)
{
	size_t start = line.find_first_not_of(' ', 0);
	size_t pos = line.find(' ', start);
	method = line.substr(start, pos);
	if (method.empty())
		return (1);

	start = line.find_first_not_of(' ', pos);
	pos = line.find(' ', start);
	target = line.substr(start, pos - start);
	if (target.empty())
		return (2);

	start = line.find_first_not_of(' ', pos);
	pos = line.find(' ', start);
	version = line.substr(start, pos);
	if (version.empty())
		return (3);

	return (0);
}

int checkVersion(std::string version, t_start &startLine)
{
	size_t pos = version.find('/', 0);
	std::string protocol = version.substr(0, pos);
	std::string ver = version.substr(pos + 1, std::string::npos);
	if ((protocol.compare("http") || protocol.compare("HTTP")) && ver.compare("1.1") )
		return (0);
	startLine.httpVersion = version;
	return (1);
}

int	treatFirstLine(std::string file, t_start &startLine)
{
	std::string method;
	std::string	target;
	std::string version;

	if (parseFirstLine(file, method, target, version))
	{
		std::cout << "error while parsing check the startup line\n";
		return (0);
	}
	if (!checkMethod(method, startLine))
		return (0);
	if (!checkRequestTarget(target, startLine))
		return (0);
	if (!checkVersion(version, startLine))
		return (0);
	return (1);
}

int parseHeaderField(std::list<headerField> &list, std::string line)
{
	headerField	field;
	size_t		start = 0;
	size_t		pos = line.find(':', start);

	field.key = line.substr(start, pos);
	start = line.find_first_not_of(' ', pos + 1);
	field.value = line.substr(start, std::string::npos);
	list.push_back(field);
	return (1);
}

int treatHeaderField(t_request newRequest, std::list<headerField> headerList)
{
}

int main(int ac, char **av)
{
	std::ifstream fileRequest("request.txt");
	std::string line;
	std::list<headerField> headerList;
	t_request	newRequest;

	if (!fileRequest.is_open())
		return 1;
	getline(fileRequest, line);
	if (!treatFirstLine(line, newRequest.startLine))
		return 1;
	while (getline(fileRequest, line) && !line.empty())
	{
		if (!parseHeaderField(headerList, line))
			return (0);
		
	}
	treatHeaderField(newRequest, headerList);
	// if (line.c_str() && newRequest.startLine.method == "POST")
	// {
	// 	std::ofstream body;
	// 	getline(fileRequest, line);
	// 	if (line.c_str())
	// 		body << line;
	// }
}