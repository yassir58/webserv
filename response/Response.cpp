/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/04 17:06:43 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/02/19 13:42:09 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response(Request &request, Config *config)
{
	printf("response addres: %p %d\n", this, request.getStatusCode());
	setRequest(&request, config);
	if (this->request->getRedirectionStatus())
	{
		this->request->setStatusCode(handleRedirection());
	}
	else if (this->request->getUploadStatus())
	{
		int code = this->request->getStatusCode();
		this->request->setStatusCode(code);
	}
	else
		applyMethod();
	getStatusCode();
	if (this->request->getStatusCode() >= BAD_REQUEST)
		handleErrorPages();
	responseToSend.push_back(generateStatusLine());
	stringContainer headerFields = generateHeaderFields(responseBody);
	responseToSend.insert(responseToSend.begin() + 1, headerFields.begin(), headerFields.end());
	responseToSend.push_back(responseBody);
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Response::~Response() {}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

/*
** --------------------------------- METHODS ----------------------------------
*/

std::string Response::generateErrorPage()
{
	std::string output;
	std::string title;
	std::string message;

	title = "Error Page - " + int2assci(status.code) + " " + statusCodeMap[status.code];
	message = int2assci(status.code) + " " + statusCodeMap[status.code];
	output = readContent("./assets/template.html");
	if (output.length() > 0)
	{
		output.replace(output.find("%TITLE"), 6, title);
		output.replace(output.find("%MESSAGE"), 8, message);
		output.replace(output.find("%VERSION"), 8, SERVER_SOFTWARE_VERSION);
	}
	return (output);
}

void	Response::handleErrorPages(void)
{
	if (this->request->getServerInstance() && this->request->getServerInstance()->getErrorPagesStatus())
	{
		std::string errorPage;
		if (this->request->getStatusCode() == NOT_FOUND)
		{
			errorPage = this->request->getServerInstance()->getErrorPages()->path_not_found;
			if (!errorPage.empty())
			{
				responseBody = readContent(errorPage);
				return ;
			}
		}
		else if (this->request->getStatusCode() == FORBIDDEN)
		{
			errorPage = this->request->getServerInstance()->getErrorPages()->path_forbidden;
			if (!errorPage.empty())
			{
				responseBody = readContent(errorPage);
				return ;
			}
		}
		else if (this->request->getStatusCode() == SERVER_ERROR)
		{
			errorPage = this->request->getServerInstance()->getErrorPages()->path_internal_error;
			if (!errorPage.empty())
			{
				responseBody = readContent(errorPage);
				return ;
			}
		}
	}
	responseBody = generateErrorPage();
}

int	Response::handleRedirection(void)
{
	std::stringstream ss;
	int					statusCode;
	ss << request->getRedirectionCode();
	ss >> statusCode;
	return (statusCode);
}

void	Response::getStatusCode(void)
{
	std::map<int, std::string>::iterator end;
	std::map<int, std::string>::iterator begin;
	end = statusCodeMap.end();
	begin = statusCodeMap.find(request->getStatusCode());
	if (begin == end)
	{
		status.code = SERVER_ERROR;
		status.status = statusCodeMap[SERVER_ERROR];
	}
	else
	{
		status.code = begin->first;
		status.status = begin->second;
	}
	
}

std::string Response::generateStatusLine(void)
{
	std::string	toReturn = "HTTP/1.1 " + request->getErrorCode() + " " + status.status + "\r\n";
	return (toReturn);
}

std::string	generateDate(void)
{
	time_t t;
    struct tm *tmp;
	char MY_TIME[50];
	std::string st;
	std::string toReturn;

	time(&t);
	tmp = localtime( &t );
	std::strftime(MY_TIME, 50, "%a, %d %b %Y %H:%M:%S GMT\r\n", tmp);
	st = MY_TIME;
	toReturn = "Date: " + st;
	return (toReturn);
}

std::string generateLenghtContent(std::string &responseBody)
{
	std::stringstream	ss;
	std::string			lenght;
	std::string			toReturn;
	ss << responseBody.length();
	ss >> lenght;
	toReturn = "Content-Lenght: " + lenght + "\r\n";
	return (toReturn);
}

std::string	Response::generateContentType(void)
{
	mapContainer mimeType = configData->getMimeMap();
	std::string extension;
	std::string path = request->getPath();
	size_t		dot;
	dot = path.find_last_of('.', std::string::npos);
	if (errorPagestatus)
		return ("Content-Type: text/html\r\n");
	if (dot == std::string::npos)
		return ("Content-Type: text/plain\r\n");
	extension = path.substr(dot + 1, std::string::npos);
	mapContainer::iterator pos = mimeType.find(extension);
	if (pos == mimeType.end())
		return ("Content-Type: text/plain\r\n");
	return ("Content-Type: " + pos->second + "\r\n");
}

stringContainer Response::generateHeaderFields(std::string &responseBody)
{
	stringContainer	toReturn;
	std::string Date = generateDate();
	toReturn.push_back(Date);
	if (!responseBody.empty())
	{
		std::string Lenght = generateLenghtContent(responseBody);
		toReturn.push_back(Lenght);
		std::string type = generateContentType();
		if (request->getListingStatus() || request->getStatusCode() >= BAD_REQUEST)
			type = "Content-Type: text/html\r\n";
		toReturn.push_back(type);
	}
	if (status.code == NOT_ALLOWED)
	{
		std::string allow = "Allow: GET, HEAD, DELETE\r\n";
		toReturn.push_back(allow);
	}
	if (request->getRedirectionStatus())
	{
		std::string location = "Location: " + request->getRedirectionLink() + " \r\n";
		toReturn.push_back(location);
	}
	std::string Server = "Server: websrv\r\n\r\n";
	toReturn.push_back(Server);
	return (toReturn);
}

std::string listDirectory (std::string dirPath)
{
	DIR *dir = opendir (dirPath.c_str ());
	struct dirent *dp;
	stringContainer dirIndex;
	stringContainer::iterator it;
	std::string responseBody("") ;
	std::string styles ("<style> body { background-color: #F2F2F2;}h1 {color: #0A2647;margin-left: 40px;} a {color:#5463FF; } </style>");
	std::string indexHeader("<h1> Index of");
	std::string lineBreak ("</br>");
	std::string line("<hr>");
	std::string lsOpen ("<ul>");
	std::string lsClose ("</ul>");
	std::string liOpen ("<li>");
	std::string liClose ("</li>");
	std::string linkOpen ("<a href=\"");
	std::string linkClose ("</a>");
	std::string hrClose ("\">");
	std::string indexBody;
	std::string href ("./");

	if (dir == NULL)
		throw std::exception ();
	else
	{
		while ((dp = readdir (dir)))
		{
			dirIndex.push_back (dp->d_name);
		}
	}
	responseBody.append (styles).append(indexHeader).append (dirPath) .append("</h1>").append (lineBreak).append (line).append (lsOpen);
	for (it = dirIndex.begin (); it != dirIndex.end (); it++)
	{
		href.append ((*it));
		responseBody.append (liOpen).append (linkOpen).append(href).append (hrClose).append ((*it)).append (linkClose).append (liClose);
		href = "./";
	}
	responseBody.append (lsClose);
	closedir (dir);
	return (responseBody);
}

int	Response::applyMethod(void)
{
	int			statusCode = request->getStatusCode();
	if (statusCode)
		return (0);
	std::string method = request->getMethod();
	if (request->getListingStatus() && statusCode == 0)
	{
		try {
			responseBody = listDirectory(request->getPath());
		}
		catch (std::exception &e)
		{
			request->setStatusCode(SERVER_ERROR);
		}
		return (0);
	}
	if (method == "GET" && statusCode == 0)
	{
		std::ifstream resource(request->getPath());
		if (resource.is_open())
		{
			std::ostringstream ss;
			ss << resource.rdbuf();
			responseBody = ss.str();
			request->setStatusCode(OK);
			resource.close();
		}
		else
			request->setStatusCode(SERVER_ERROR);
	}
	else if (method == "POST" && statusCode == 0)
	{
		std::ofstream outfile;

		outfile.open (request->getPath(), std::ios::binary);
		if (outfile.is_open())
		{
			outfile << request->getBody();
			outfile.close();
			request->setStatusCode(OK);
		}
		else
			request->setStatusCode(SERVER_ERROR);
	}
	else if (method == "DELETE" && statusCode == 0)
	{
		if (request->getPath().find("..", 0) != std::string::npos)
			return (request->setStatusCode(FORBIDDEN), 1);
		if (request->getRoot().empty() || request->getRoot().find("/www") == std::string::npos)
			return (request->setStatusCode(FORBIDDEN), 1);
		if (remove(request->getPath().c_str()))
			request->setStatusCode(SERVER_ERROR);
		else
			request->setStatusCode(OK);
	}
	return (0);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string Response::getResponse(void)
{
	std::string toReturn;
	std::stringstream ss;
	stringContainer::iterator begin = responseToSend.begin();
	stringContainer::iterator end = responseToSend.end();
	while (end != begin)
	{
		ss << *begin;
		++begin;
	}
	toReturn = ss.str();
	return (toReturn);
}

void	Response::setRequest(Request *request, Config *config)
{
	this->request = request;
	errorPagestatus = false;
	this->configData = config;
	size_t	index = 0;
	statusCodeMap[OK] = "OK";
	statusCodeMap[CREATED] = "CREATED";
	statusCodeMap[FOUND] = "FOUND";
	statusCodeMap[NO_CONTENT] = "NO_CONTENT";
	statusCodeMap[BAD_REQUEST] = "Bad Request";
	statusCodeMap[FORBIDDEN] = "Forbidden";
	statusCodeMap[NOT_FOUND] = "Not Found";
	statusCodeMap[NOT_ALLOWED] = "Method Not Allowed";
	statusCodeMap[TOO_LONG] = "Request-URI Too Long";
	statusCodeMap[TOO_LARGE] = "Request Entity Too Large";
	statusCodeMap[NOT_IMPLENTED] = "Not Implemented";
	statusCodeMap[SERVER_ERROR] = "Internal Server Error";
	statusCodeMap[HTTP_VERSION] = "HTTP Version Not Supported";
}

void	Response::setResponseBody(std::string responseBody)
{
	this->responseBody = responseBody;
}

/* ************************************************************************** */