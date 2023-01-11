/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/13 15:35:19 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/10 19:29:41 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./request/request.h"
#include "./request/Request.hpp"
#include "./response/Response.hpp"

int main(void)
{
	char fileRequest[] = {"GET http//127.1.1.1:00/rfc/rfc2616/ HTTP/1.1\r\nHost: 192.120.18.0:80\r\nConnection: keep-alive\r\n\r\nsomething"};
	Request newRequest(fileRequest);
	std::string body = "sbardila hoho teyoo jotaro";
	Response	newResponse(newRequest, body);
	stringContainer::iterator end = newResponse.responseToSend.end();
	stringContainer::iterator begin = newResponse.responseToSend.begin();
	std::cout << "===============request==============\n";
	newRequest.printResult();
	std::cout << "IP address: " << newRequest.getStartLine().IpAdress << std::endl;
	std::cout << "Port: " << newRequest.getStartLine().Port << std::endl;
	std::cout << "\n===============response==============\n";
	for (; end != begin; ++begin)
	{
		std::cout << *begin;
	}
	//decide if cgi or not
	// std::string line;
	// for (int i; i < 10; ++i)
	// {
	// 	std::cout << newRequest.getCRLF(line, (char *)"\n") << std::endl;
	// 	std::cout << line << std::endl;
	// }
}