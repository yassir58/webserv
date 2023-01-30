/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 20:24:14 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/30 18:23:23 by Ma3ert           ###   ########.fr       */
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
	if (getCRLF(line, (char *)"\r\n"))
	{
		statusCode = BAD_REQUEST;
		return ;
	}
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
		{
			statusCode = BAD_REQUEST;
			return ;
		}
	}
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
			{
				statusCode = BAD_REQUEST;
				return ;
			}
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
	std::string	defaultCGI = pathLocation->getCGIDefault();
	size_t		dot;
	if (pathLocation->getCGIStatus() == false || extension.empty())
		return (false);
	dot = path.find_last_of('.', std::string::npos);
	if (dot == std::string::npos)
		return (false);
	fileExtension = path.substr(dot + 1, std::string::npos);
	if (fileExtension == extension)
	{
		if (defaultCGI.empty())
			return (false);
		defaultCGI = adjustPath(root, defaultCGI);
		if (access(defaultCGI.c_str(), F_OK) == -1)
		{
			statusCode = NOT_FOUND;
			return (false);
		}
		if (access(defaultCGI.c_str(), X_OK) == -1 || access(path.c_str(), X_OK) == -1)
		{
			statusCode = FORBIDDEN;
			return (false);
		}
		return (true);
	}
	return (false);
}

int isDir(const char* fileName)
{
    struct stat path;

    stat(fileName, &path);

    return S_ISDIR(path.st_mode);
}

int	Request::checkDirectory(Location *pathLocation)
{
	int dec = isDir(path.c_str());
	if (dec)
	{
		std::string indexFile = pathLocation->getDefaultIndex();
		if (!indexFile.empty())
		{
			path = adjustPath(path, indexFile);
			return (0);
		}
		if (pathLocation->getListingStatus())
		{
			listingStatus = true;
			return (0);
		}
		statusCode = FORBIDDEN;
	}
	return (1);
}

bool	Request::checkUpload(Location *pathLocation)
{
	if (pathLocation->getUploadStatus() && startLine.method == "POST")
	{
		headerField *type = getHeaderField("Content-Type");
		if (!type)
			return (true);
		std::string uploadPath = pathLocation->getUploadPath();
		if (!uploadPath.empty())
		{
			size_t pos = path.find_last_of('/', std::string::npos);
			std::string fileName = path.substr(pos, std::string::npos);
			path = path.substr(0, pos);
			path = adjustPath(path, uploadPath);
			std::cout << "path: " << path << std::endl;
			if (access(path.c_str(), F_OK) == -1)
			{
				std::cout << "hoho\n";
				statusCode = NOT_FOUND;
				return (false);
			}
			path = adjustPath(path, fileName);
		}
		if (type->value == "multipart/form-data")
		{
			std::cout << "path: " << path << std::endl;
			std::ofstream file;
			file.open(path, std::ofstream::out | std::ofstream::trunc);
			if (!file.is_open())
			{
				statusCode = NOT_FOUND;
				return (false);
			}
			if (access(path.c_str(), W_OK) == -1)
			{
				statusCode = FORBIDDEN;
				return (false);
			}
			file << getBody();
			file.close();
			upload = true;
			statusCode = CREATED;
		}
	}
	return (true);
}

bool Request::checkCGI(void)
{
	this->pathLocation = matchLocation();
	if (!this->pathLocation)
	{
		statusCode = NOT_FOUND;
		return (false);
	}
	if (serverInstance->getMaxBodySize() > 0 && getBody().length() > serverInstance->getMaxBodySize())
	{
		statusCode = TOO_LARGE;
		return (false);
	}
	this->root = serverInstance->getRoot();
	if (!pathLocation->getRoot().empty())
		root = pathLocation->getRoot();
	path = adjustPath(root, path);
	if (!checkDirectory(pathLocation))
		return (false);
	if(!checkUpload(pathLocation))
		return (false);
	if (!treatAbsolutePath(pathLocation))
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

int Request::treatAbsolutePath(Location *pathLocation)
{
	stringContainer methods = pathLocation->getMethods();
	if (access(path.c_str(), F_OK) == -1)
	{
		statusCode = NOT_FOUND;
		return (0);
	}
	if (methods.size() != 0)
	{
		stringContainer::iterator begin = methods.begin();
		stringContainer::iterator end = methods.end();
		for (;begin != end; ++begin)
		{
			if ((*begin) == startLine.method)
				break ;
		}
		if (begin == end)
		{
			statusCode = NOT_ALLOWED;
			return (0);
		}
	}
	if (startLine.method == "POST" && access(path.c_str(), W_OK) == -1)
	{
		statusCode = FORBIDDEN;
		return (0);
	}
	if (startLine.method == "GET" && access(path.c_str(), R_OK) == -1)
	{
		statusCode = FORBIDDEN;
		return (0);
	}
	return (1);
}

int Request::checkRequestTarget()
{
	if (!treatAbsoluteURI())
		return (0);
	path = startLine.requestTarget;
	Server *serverInst = ;
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

std::string Request::adjustPath(std::string const &prefix, std::string const &sufix)
{
	std::string toReturn;
	if ((*sufix.begin()) == '/' && (*(prefix.end() - 1)) == '/')
	{
		toReturn = sufix.substr(1, std::string::npos);
		toReturn = prefix + toReturn;
		return (toReturn);
	}
	else if ((*sufix.begin() != '/') && (*(prefix.end() - 1) != '/'))
	{
		toReturn = prefix + "/" + sufix;
		return (toReturn);
	}
	toReturn = prefix + sufix;
	return (toReturn);
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
	redirectionStatus = false;
	listingStatus = false;
	upload = false;
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

stringContainer		Request::getBodyStringContainer(void)
{
	return (body);
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

bool			Request::getRedirectionStatus(void)
{
	return (redirectionStatus);
}

std::string		Request::getRedirectionLink(void)
{
	return (redirectionLink);
}

std::string		Request::getRedirectionCode(void)
{
	return (redirectCode);
}

bool		Request::getListingStatus(void)
{
	return (listingStatus);
}

bool		Request::getUploadStatus(void)
{
	return (upload);
}

Location *Request::getLocation(void)
{
	return (pathLocation);
}

/* ************************************************************************** */