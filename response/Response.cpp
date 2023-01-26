/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/04 17:06:43 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/25 20:24:04 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response(Request &request)
{
	setRequest(&request);
	if (this->request->getRedirectionStatus())
		this->request->setStatusCode(handleRedirection());
	else
		applyMethod();
	statusIndex = getStatusCode();
	responseToSend.push_back(generateStatusLine());
	stringContainer headerFields = generateHeaderFields(responseBody);
	responseToSend.insert(responseToSend.begin() + 1, headerFields.begin(), headerFields.end());
	responseToSend.push_back(responseBody);
}

Response::Response(Request &request, std::string CGIOutput)
{
	setRequest(&request);
	setResponseBody(CGIOutput);
	statusIndex = getStatusCode();
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

int	Response::handleRedirection(void)
{
	std::stringstream ss;
	int					statusCode;
	ss << request->getRedirectionCode();
	ss >> statusCode;
	return (statusCode);
}

int	Response::getStatusCode(void)
{
	size_t index = 0;
	int		code = request->getStatusCode();
	while (code != status[index].code)
		++index;
	return (index);
}

std::string Response::generateStatusLine(void)
{
	std::string	toReturn = "HTTP/1.1 " + request->getErrorCode() + " " + status[statusIndex].status + "\r\n";
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

stringContainer Response::generateHeaderFields(std::string &responseBody)
{
	stringContainer	toReturn;
	std::string Date = generateDate();
	toReturn.push_back(Date);
	std::string Lenght = generateLenghtContent(responseBody);
	toReturn.push_back(Lenght);
	// I still need to work on the content type
	if (this->getStatusCode() == NOT_ALLOWED)
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

int	Response::applyMethod(void)
{
	std::string method = request->getMethod();
	int			statusCode = request->getStatusCode();
	if (method == "GET" && statusCode == 0)
	{
		std::ifstream resource(request->getPath());
		if (resource)
		{
			std::ostringstream ss;
			ss << resource.rdbuf();
			responseBody = ss.str();
			request->setStatusCode(OK);
		}
		else
			request->setStatusCode(SERVER_ERROR);
	}
	else if (method == "POST" && statusCode == 0)
	{
		std::ofstream outfile(request->getPath());
		if (outfile)
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

void	Response::setRequest(Request *request)
{
	this->request = request;
	size_t	index = 0;
	status[index].code = OK; status[index++].status = "OK";
	status[index].code = CREATED; status[index++].status = "CREATED";
	status[index].code = FOUND; status[index++].status = "FOUND";
	status[index].code = NO_CONTENT; status[index++].status = "NO_CONTENT";
	status[index].code = BAD_REQUEST; status[index++].status = "BAD_REQUEST";
	status[index].code = FORBIDDEN; status[index++].status = "FORBIDDEN";
	status[index].code = NOT_FOUND; status[index++].status = "NOT_FOUND";
	status[index].code = NOT_ALLOWED; status[index++].status = "NOT_ALLOWED";
	status[index].code = NOT_IMPLENTED; status[index++].status = "NOT_IMPLENTED";
	status[index].code = SERVER_ERROR; status[index++].status = "SERVER_ERROR";
	status[index].code = HTTP_VERSION; status[index++].status = "HTTP_VERSION";
}

void	Response::setResponseBody(std::string responseBody)
{
	this->responseBody = responseBody;
}

/* ************************************************************************** */