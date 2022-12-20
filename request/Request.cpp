/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 20:24:14 by Ma3ert            #+#    #+#             */
/*   Updated: 2022/12/20 21:16:47 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request(std::ifstream &file)
{
	std::string line;
	getline(file, line);
	if (parseFirstLine(line))
	{
		std::cout << "error while parsing check the startup line\n";
		errorCode = 0;
	}
	if (!checkMethod())
		errorCode = 100;
	if (!checkRequestTarget())
		errorCode = 200;
	if (!checkVersion())
		errorCode = 300;
	while (getline(file, line) && !line.empty())
	{
		if (!parseHeaderField(headerFields, line))
			errorCode = 400;
		
	}
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Request::~Request()
{
}

/*
** --------------------------------- METHODS ----------------------------------
*/

int Request::checkMethod()
{
	if (!startLine.method.compare("GET"))
	{
		startLine.method = "GET";
		return (1);
	}
	if (!startLine.method.compare("POST"))
	{
		startLine.method = "POST";
		return (1);
	}
	if (!startLine.method.compare("DELETE"))
	{
		startLine.method = "DELETE";
		return (1);
	}
	return (0);
}

int	Request::treatAbsoluteURI()
{
	if (!startLine.requestTarget.compare(0, 6, "http//") || !startLine.requestTarget.compare(0, 6, "HTTP//"))
	{
		startLine.requestTarget.erase(0, 6);
		size_t pos = startLine.requestTarget.find('/', 0);
		startLine.hostName = startLine.requestTarget.substr(0, pos);
		startLine.requestTarget.erase(0, pos);
		startLine.requestTarget = startLine.requestTarget.substr(0, std::string::npos);
		return (1);
	}
	return (0);
}

int Request::treatAbsolutePath()
{
	return (1);
}

int Request::checkRequestTarget()
{
	if (treatAbsoluteURI())
		return (1);
	if (treatAbsolutePath())
		return (1);
	return (0);
}

int Request::parseFirstLine(std::string line)
{
	size_t start = 0;
	size_t pos = line.find(' ', start);
	startLine.method = line.substr(start, pos);
	if (startLine.method.empty())
		return (1);

	start = pos + 1;
	pos = line.find(' ', start);
	startLine.requestTarget = line.substr(start, pos - start);
	if (startLine.requestTarget.empty())
		return (2);

	start = pos + 1;
	pos = line.find(' ', start);
	startLine.httpVersion = line.substr(start, pos);
	if (startLine.httpVersion.empty())
		return (3);

	return (0);
}

int Request::checkVersion()
{
	size_t pos = startLine.httpVersion.find('/', 0);
	std::string protocol = startLine.httpVersion.substr(0, pos);
	std::string ver = startLine.httpVersion.substr(pos + 1, std::string::npos);
	if ((protocol.compare("http") || protocol.compare("HTTP")) && ver.compare("1.1") )
		return (0);
	startLine.httpVersion = startLine.httpVersion;
	return (1);
}


int Request::parseHeaderField(std::list<headerField> &list, std::string line)
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

void	Request::printResult(void)
{
	std::cout << "method: " << startLine.method << std::endl;
	std::cout << "tartget: " << startLine.requestTarget << std::endl;
	std::cout << "http version: " << startLine.httpVersion << std::endl;
	std::cout << std::endl;
	std::list<headerField>::iterator end = headerFields.end();
	std::list<headerField>::iterator begin = headerFields.begin();
	for (; begin != end; ++begin)
	{
		std::cout << "key: " << begin->key << std::endl;
		std::cout << "value: " << begin->value << std::endl;
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::list<headerField> &Request::getHeaderField(void) 
{
	return (headerFields);
}

t_start &Request::getStartLine(void)
{
	return (startLine);
}

/* ************************************************************************** */