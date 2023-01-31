/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/31 15:58:58 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/31 15:58:59 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

int	Request::parseRequest(serverBlocks serverList, std::vector<int> resolversList)
{
	std::string line;
	if (getCRLF(line, (char *)"\r\n"))
	{
		statusCode = BAD_REQUEST;
		return (0);
	}
	if (!parseFirstLine(line))
	{
		statusCode = BAD_REQUEST;
		return (0);
	}
	if (!checkContentParsed(serverList, resolversList))
		return (0);
	while (!getCRLF(line, (char *)"\r\n"))
	{
		if (!parseHeaderField(headerFields, line))
		{
			statusCode = BAD_REQUEST;
			return (0);
		}
	}
	if (!startLine.Query.empty())
		startLine.requestTarget = startLine.requestTarget +  "?" + startLine.Query;
	if (startLine.Host == false && startLine.IpAdress.empty())
	{
		statusCode = BAD_REQUEST;
		return (0);
	}
	if (startLine.method == "POST")
	{
		while (!getCRLF(line, (char *)"\n"))
		{
			if (!parseBody(line))
			{
				statusCode = BAD_REQUEST;
				return (0);
			}
		}
	}
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

int	Request::checkContentParsed(serverBlocks serverList, std::vector <int> resolversList)
{
	if (!checkMethod())
	{
		statusCode = NOT_IMPLENTED;
		return (0);
	}
	if (!checkRequestTarget(serverList, resolversList))
	{
		if (statusCode == 0)
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

int Request::checkRequestTarget(serverBlocks serverList, std::vector <int> resolversList)
{
	if (!treatAbsoluteURI())
		return (0);
	path = startLine.requestTarget;
	Server *serverInst = matchRequestHandler (serverList, resolversList);
	setServerInstance(serverInst);
	Location *pathLocation = matchLocation();
	if (pathLocation == NULL)
	{
		statusCode = NOT_FOUND;
		return (0);
	}
	std::string redirect = pathLocation->getRedirectLink();
	std::string redirCode = pathLocation->getRedirectCode();
	if (!redirect.empty())
	{
		redirectionLink = redirect;
		this->redirectCode = redirCode;
		redirectionStatus = true;
		return (0);
	}
	return (1);
}

int	Request::treatAbsoluteURI()
{
	std::string hostName;
	if (startLine.requestTarget.length() > 1000)
	{
		statusCode = TOO_LONG;
		return (0);
	}
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