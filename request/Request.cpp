/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 20:24:14 by Ma3ert            #+#    #+#             */
/*   Updated: 2022/12/25 22:19:46 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request(std::string fileString)
{
	// std::string line;
	setFileString(fileString);
	// getCRLF(line, (char *)"\r\n");
	// if (parseFirstLine(line))
	// {
	// 	std::cout << "error while parsing check the startup line\n";
	// 	errorCode = 0;
	// }
	// if (!checkMethod())
	// 	errorCode = 400;
	// if (!checkRequestTarget())
	// 	errorCode = 400;
	// if (!checkVersion())
	// 	errorCode = 400;
	// while (!getCRLF(line, (char *)"\r\n"))
	// {
	// 	std::cout << "second: " << line << std::endl;
	// 	if (!parseHeaderField(headerFields, line))
	// 		errorCode = 400;
	// }
	// while (!getCRLF(line, (char *)"\n"))
	// {
	// 	std::cout << "last: " << line << std::endl;
	// 	if (parseBody(line))
	// 		errorCode = 400;
	// }
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

int	Request::getCRLF(std::string &newLine, char *delim)
{
	static size_t pos;
	static size_t start;
	pos = fileString.find(delim, start);
	newLine = fileString.substr(start, pos - start);
	if (pos != std::string::npos)
	{
		start = pos + strlen(delim);
		start = fileString.length();
		std::cout << "I got here\n";
	}
	return (newLine.empty());
}

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
	errorCode = 405;
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
	{
		errorCode = 505;
		return (0);
	}
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

int	Request::parseBody(std::string line)
{
	body.push_back(line);
	body.push_back("\n");
	return (1);
}

void	Request::printResult(void)
{
	std::cout << "method: " << startLine.method << std::endl;
	std::cout << "tartget: " << startLine.requestTarget << std::endl;
	std::cout << "http version: " << startLine.httpVersion << std::endl;
	std::cout << "\n===============this headerField===============\n" << std::endl;
	std::list<headerField>::iterator end = headerFields.end();
	std::list<headerField>::iterator begin = headerFields.begin();
	for (; begin != end; ++begin)
	{
		std::cout << "key: " << begin->key << std::endl;
		std::cout << "value: " << begin->value << std::endl;
	}
	std::cout << "\n=============this body field===============\n";
	std::vector<std::string>::iterator vend = body.end();
	std::vector<std::string>::iterator vbegin = body.begin();
	for (; vbegin != vend; ++vbegin)
	{
		std::cout << *vbegin;
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

void	Request::setFileString(std::string &file)
{
	fileString = file;
}

/* ************************************************************************** */