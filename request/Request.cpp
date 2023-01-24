/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 20:24:14 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/24 19:03:06 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"


/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request(std::string fileString, Server *serverInst)
{
	std::string line;
	setStatusCode(0);
	setFileString(fileString);
	setServerInstance(serverInst);
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
	path = startLine.requestTarget;
	if (!startLine.Query.empty())
		startLine.requestTarget = startLine.requestTarget +  "?" + startLine.Query;
	if (startLine.Host == false && startLine.IpAdress.empty())
	{
		statusCode = BAD_REQUEST;
		return ;
	}
	if (startLine.method == "POST")
	{
		while (!getCRLF(line, (char *)"\n"))
		{
			if (!parseBody(line))
				statusCode = BAD_REQUEST;
		}
	}
	this->CGI = checkCGI();
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

Location *Request::matchLocation(void)
{
	std::vector<Location *> Locations = serverInstance->getLocations();
	std::vector<Location *>::iterator begin = Locations.begin();
	std::vector<Location *>::iterator end = Locations.end();
	size_t	pos;
	while (end != begin)
	{
		pos = path.find((*begin)->getEndPoint(), 0);
		if (pos == 0)
			return (*begin);
		++begin;
	}
	return (NULL);
}

bool Request::checkExtension(Location *pathLocation)
{
	std::string	extension = pathLocation->getCGIExtension();
	std::string	fileExtension;
	size_t		dot;
	if (pathLocation->getCGIStatus() == false || extension.empty())
		return (false);
	dot = path.find_last_of('.', std::string::npos);
	if (dot == std::string::npos)
		return (false);
	fileExtension = path.substr(dot + 1, std::string::npos);
	if (fileExtension == extension)
		return (true);
	return (false);
}

bool Request::checkCGI(void)
{
	Location *pathLocation = matchLocation();
	std::string root = serverInstance->getRoot();
	if (!pathLocation->getRoot().empty())
		root = pathLocation->getRoot();
	if (path.front() == '/' && (*root.end()) == '/')
		path = path.substr(1, std::string::npos);
	path = root + path;
	std::cout << "path: " << path << std::endl;
	if (!treatAbsolutePath())
		return (false);
	return (checkExtension(pathLocation));
}

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
	std::string hostName;
	if (!startLine.requestTarget.compare(0, 7, "http://") || !startLine.requestTarget.compare(0, 7, "HTTP://"))
	{
		startLine.requestTarget.erase(0, 7);
		size_t pos = startLine.requestTarget.find('/', 0);
		hostName = startLine.requestTarget.substr(0, pos);
		startLine.requestTarget.erase(0, pos);
		startLine.requestTarget = startLine.requestTarget.substr(0, std::string::npos);
		parseHostName(hostName);
		return (1);
	}
	return (0);
}

int Request::treatAbsolutePath()
{
	if (access(path.c_str(), F_OK) == -1)
	{
		statusCode = NOT_FOUND;
		return (0);
	}
	if (startLine.method == "POST" && access(path.c_str(), W_OK) == -1)
	{
		statusCode = NOT_ALLOWED;
		return (0);
	}
	if (startLine.method == "GET" && access(path.c_str(), R_OK) == -1)
	{
		statusCode = NOT_ALLOWED;
		return (0);
	}
	return (1);
}

int Request::checkRequestTarget()
{
	treatAbsoluteURI();
	return (1);
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

void	Request::parseHostName(std::string &hostNameValue)
{
	if (startLine.Host == true || !startLine.Port.empty())
		return ;
	size_t queryPos = this->startLine.requestTarget.find('?', 0);
	if (queryPos != std::string::npos)
	{
		startLine.Query = this->startLine.requestTarget.substr(queryPos + 1, std::string::npos);
		this->startLine.requestTarget = this->startLine.requestTarget.substr(0, queryPos);
	}
	if (isdigit (hostNameValue[0]) || !hostNameValue.compare(0, 9, "localhost"))
	{
		size_t pos = hostNameValue.find(':', 0);
		startLine.IpAdress = hostNameValue.substr(0, pos);
		if (pos == std::string::npos)
			startLine.Port = "80";
		else
			startLine.Port = hostNameValue.substr(pos + 1, std::string::npos);
	}
	else
	{
		startLine.Host = true;
		size_t pos1 = hostNameValue.find(':', 0);
		if (pos1 != std::string::npos)
		{
			startLine.hostName = hostNameValue.substr(0, pos1);
			startLine.Port = hostNameValue.substr(pos1 + 1, std::string::npos);
		}
		else
			startLine.hostName = hostNameValue;
	}
}

int Request::parseHeaderField(headerFieldList &list, std::string line)
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
	if (field.key == "Host" && startLine.hostName.empty())
		parseHostName(field.value);
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
	headerFieldList::iterator end = headerFields.end();
	headerFieldList::iterator begin = headerFields.begin();
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

headerFieldList &Request::getHeaderFieldlist(void) 
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
	startLine.Host = false;
	pos = 0;
	start = 0;
}

void	Request::setStatusCode(int newStatusCode)
{
	statusCode = newStatusCode;
}

void	Request::setServerInstance(Server *server)
{
	serverInstance = server;
	CGI = false;
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

std::string Request::getBody(void)
{
	std::string toReturn;
	std::stringstream ss;
	stringContainer::iterator begin = body.begin();
	stringContainer::iterator end = body.end();
	while (end != begin)
	{
		ss << *begin;
		++begin;
	}
	toReturn = ss.str();
	return (toReturn);
}

headerField *Request::getHeaderField(std::string key)
{
	headerFieldList::iterator begin = headerFields.begin();
	headerFieldList::iterator end = headerFields.end();
	while (begin != end)
	{
		if (begin->key == key)
			return (&(*begin));
		++begin;
	}
	return (NULL);
}

bool Request::getCGIStatus(void)
{
	return (CGI);
}

std::string Request::getPath(void)
{
	return (path);
}

/* ************************************************************************** */