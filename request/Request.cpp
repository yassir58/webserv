/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 20:24:14 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/02/12 15:24:45 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"


/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request(Connection *newConnection)
{
	setStatusCode(0);
	configFile = newConnection->getConfig();
	setFileString(newConnection->getRequestHeaders(), newConnection->getRequestBody());
	if (!parseRequest(newConnection->getServerBlocks(), newConnection->getResolversList()))
		return ;
	this->CGI = checkLocationPath();
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

bool Request::checkLocationPath(void)
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

Location *Request::matchLocation(void)
{
	std::vector<Location *> Locations = serverInstance->getLocations();
	if (!Locations.size())
		return (NULL);
	std::vector<Location *>::iterator begin = Locations.begin();
	std::vector<Location *>::iterator end = Locations.end();
	size_t	pos;
	std::cout << "Location: " << Locations.size() << std::endl;
	while (end != begin)
	{
		int dec = path.compare((*begin)->getEndPoint());
		if (!dec)
			return (*begin);
		++begin;
	}
	begin = Locations.begin();
	while (end != begin)
	{
		pos = path.find((*begin)->getEndPoint(), 0);
		if (pos == 0)
			return (*begin);
		++begin;
	}
	return (NULL);
}

int	Request::checkDirectory(Location *pathLocation)
{
	int dec = isDir(path.c_str());
	if (dec && startLine.method == "GET")
	{
		if (*(path.end() - 1) != '/')
		{
			std::cout << "hoho///////////////\n";
			size_t pos = path.find(root);
			redirectionLink = path.substr(pos + root.length(), std::string::npos) + "/";
			redirectCode = "301";
			redirectionStatus = true;
			return (0);
		}
		std::string indexFile = pathLocation->getDefaultIndex();
		if (!indexFile.empty())
		{
			path = adjustPath(path, indexFile);
			return (1);
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
		mapContainer::iterator pos = configFile->getMimeMap().find(type->value);
		if (type->value == "multipart/form-data" || pos != configFile->getMimeMap().end())
		{
			if (!uploadPath.empty())
			{
				size_t pos = path.find_last_of('/', std::string::npos);
				std::string fileName = path.substr(pos, std::string::npos);
				path = path.substr(0, pos);
				path = adjustPath(path, uploadPath);
				if (access(path.c_str(), F_OK) == -1)
				{
					statusCode = NOT_FOUND;
					return (false);
				}
				path = adjustPath(path, fileName);
			}
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
			file.write(body.data(), body.size());
			file.close();
			upload = true;
			statusCode = CREATED;
			return (false);
		}
	}
	return (true);
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
		if (access(defaultCGI.c_str(), F_OK) == -1)
		{
			statusCode = NOT_FOUND;
			return (false);
		}
		if (access(defaultCGI.c_str(), X_OK) == -1)
		{
			statusCode = FORBIDDEN;
			return (false);
		}
		return (true);
	}
	return (false);
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

void	Request::setFileString(std::string const &file, std::vector<char> const &newBody)
{
	fileString = file;
	startLine.Host = false;
	body = newBody;
	pos = 0;
	start = 0;
	CGI = false;
	redirectionStatus = false;
	listingStatus = false;
	upload = false;
}

void	Request::setStatusCode(int newStatusCode)
{
	statusCode = newStatusCode;
}

void	Request::setServerInstance(Server *server)
{
	serverInstance = server;
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
	std::vector<char>::iterator begin = body.begin();
	std::vector<char>::iterator end = body.end();
	while (end != begin)
	{
		ss << *begin;
		++begin;
	}
	toReturn = ss.str();
	return (toReturn);
}

std::vector<char>	Request::getBodyVector(void)
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

Server			*Request::getServerInstance(void)
{
	return (serverInstance);
}

/* ************************************************************************** */