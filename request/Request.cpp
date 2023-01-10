/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 20:24:14 by Ma3ert            #+#    #+#             */
<<<<<<< Updated upstream
/*   Updated: 2023/01/09 18:55:54 by Ma3ert           ###   ########.fr       */
=======
/*   Updated: 2023/01/09 14:58:47 by Ma3ert           ###   ########.fr       */
>>>>>>> Stashed changes
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request(std::string fileString)
{
	std::string line;
	setStatusCode(0);
	setFileString(fileString);
	getCRLF(line, (char *)"\r\n");
	if (!parseFirstLine(line))
	{
		statusCode = BAD_REQUEST;
		return ;
	}
	if (!checkContentParsed())
		return ;
	while (!getCRLF(line, (char *)"\r\n"))
	{
		if (!parseHeaderField(headerFields, line))
			statusCode = BAD_REQUEST;
	}
	if (startLine.method == "POST")
	{
		while (!getCRLF(line, (char *)"\n"))
		{
			if (!parseBody(line))
				statusCode = BAD_REQUEST;
		}
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

int	Request::checkContentParsed()
{
	if (!checkMethod())
	{
		statusCode = NOT_IMPLENTED;
		return (0);
	}
	if (!checkRequestTarget())
	{
		statusCode = BAD_REQUEST;
		return (0);
	}
	if (!checkVersion())
	{
		statusCode = HTTP_VERSION;
		return (0);
	}
	return (1);
}

int	Request::getCRLF(std::string &newLine, char *delim)
{
	static size_t pos;
	static size_t start;
	if (start == fileString.length())
	{
		newLine = "";
		return (1);
	}
	pos = fileString.find(delim, start);
	newLine = fileString.substr(start, pos - start);
	if (pos == std::string::npos)
	{
		pos = fileString.length();
		start = pos;
	}
	else
		start = pos + strlen(delim);
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
	// if (access(startLine.requestTarget, F_OK))
	// {
	// 	statusCode = 404;
	// }
	// if (startLine.method == "POST" && access(startLine.requestTarget, W_OK))
	// {
	// 	statusCode = 405;
	// }
	// if (startLine.method == "GET" && access(startLine.requestTarget, R_OK))
	// {
	// 	statusCode = 405;
	// }
	statusCode = 204;
	return (1);
}

int Request::checkRequestTarget()
{
	treatAbsoluteURI();
	if (treatAbsolutePath())
		return (1);
	return (0);
}

int Request::parseFirstLine(std::string line)
{
	/*----------------method----------------------*/
	size_t start = 0;
	size_t pos = line.find(' ', start);
	startLine.method = line.substr(start, pos);
	if (startLine.method.empty())
		return (0);
	/*----------------request target--------------*/
	start = pos + 1;
	pos = line.find(' ', start);
	startLine.requestTarget = line.substr(start, pos - start);
	if (startLine.requestTarget.empty())
		return (0);
	/*----------------HTTP version---------------*/
	start = pos + 1;
	pos = line.find(' ', start);
	startLine.httpVersion = line.substr(start, pos);
	if (startLine.httpVersion.empty())
		return (0);
	return (1);
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

<<<<<<< Updated upstream
int Request::parseHeaderField(headerFieldList &list, std::string line)
=======
int Request::parseHeaderField(std::list<headerField> &list, std::string line)
>>>>>>> Stashed changes
{
	headerField	field;
	size_t		start = 0;
	size_t		pos = line.find(':', start);
	if (pos == std::string::npos)
		return (0);
	field.key = line.substr(start, pos);
	start = line.find_first_not_of(' ', pos + 1);
	field.value = line.substr(start, std::string::npos);
	if (field.value.empty() || field.key.empty())
		return (0);
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
<<<<<<< Updated upstream
	headerFieldList::iterator end = headerFields.end();
	headerFieldList::iterator begin = headerFields.begin();
=======
	std::list<headerField>::iterator end = headerFields.end();
	std::list<headerField>::iterator begin = headerFields.begin();
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
headerFieldList &Request::getHeaderField(void) 
=======
std::list<headerField> &Request::getHeaderField(void) 
>>>>>>> Stashed changes
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

void	Request::setStatusCode(int newStatusCode)
{
	statusCode = newStatusCode;
}

std::string		Request::getErrorCode(void)
{
	std::stringstream	ss;
	std::string			toReturn;
	ss << statusCode;
	ss >> toReturn;
	return (toReturn);
}

std::string		Request::getMethod(void)
{
	return (startLine.method);
}

std::string		Request::getHTTPVersion(void)
{
	return (startLine.httpVersion);
}

std::string		Request::getRequestTarget(void)
{
	return (startLine.requestTarget);
}

int Request::getStatusCode(void)
{
	return (statusCode);
}

/* ************************************************************************** */