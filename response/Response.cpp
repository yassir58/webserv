/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/04 17:06:43 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/09 14:07:08 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Response::Response(Request &request, std::string &responseBody)
{
	setRequest(&request);
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
	// if (this->getStatusCode() == "405")
	// {
	// 	// treat the allow mothod
	// }
	std::string Server = "Server: websrv\r\n\r\n";
	toReturn.push_back(Server);
	return (toReturn);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

void	Response::setRequest(Request *request)
{
	this->request = request;
	size_t	index = 0;
	status[index].code = OK; status[index++].status = "OK";
	status[index].code = CREATED; status[index++].status = "CREATED";
	status[index].code = NO_CONTENT; status[index++].status = "NO_CONTENT";
	status[index].code = BAD_REQUEST; status[index++].status = "BAD_REQUEST";
	status[index].code = FORBIDDEN; status[index++].status = "FORBIDDEN";
	status[index].code = NOT_FOUND; status[index++].status = "NOT_FOUND";
	status[index].code = NOT_ALLOWED; status[index++].status = "NOT_ALLOWED";
	status[index].code = NOT_IMPLENTED; status[index++].status = "NOT_IMPLENTED";
	status[index].code = SERVER_ERROR; status[index++].status = "SERVER_ERROR";
	status[index].code = HTTP_VERSION; status[index++].status = "HTTP_VERSION";
}

/* ************************************************************************** */