/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/13 15:35:19 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/31 12:36:22 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.h"
#include "Request.hpp"
#include "../response/Response.hpp"
#include <unistd.h>
#include <iostream>

std::string requestGeneratorByGPT()
{
	std::string method = "POST";
	std::ofstream test("request");
    std::string url = "/test/img.jpeg";
    std::string headers = "Accept: application/json\r\nHost: localhost:8080\r\nConnection: keep-alive\r\naccept: /\r\naccept-encoding: gzip, deflate, br\r\ncontent-type: multipart/form-data\r\naccept-language: en-US,en;q=0.9\r\nsec-fetch-mode: cors\r\n";

    std::stringstream request;
    request << method << " " << url << " HTTP/1.1\r\n";
    request << headers;
    request << "\r\n";
    // request << body1;
    // request << body2;

    std::string request_str = request.str();

	test << request_str ;
	return (request_str);
}

int main(void)
{
	requestGeneratorByGPT ();
	try
	{
		// char fileRequest[] = {	"POST /echo/post/json HTTP/1.1\r\nHost: reqbin.com\r\nAccept: application/json\r\nContent-Type: application/json\r\nContent-Length: 81\r\n\r\n{\nId: 78912,\nCustomer: Jason Sweet,\nQuantity: 1,\nPrice: 18.00\n}\n"};
		// char fileRequest[] = {"POST index.php/tv/home?season=5&episode=62 HTTP/1.1\r\n\r\n"};
		// Config conf("../testing/configs/request.conf");
		// conf.parseConfig();
		// std::vector<Server *> ht = conf.getHttpContext()->getServers();
		// Server	*serv = (*ht.begin());
		// std::string fileRequest = requestGeneratorByGPT();
		// Request newRequest(fileRequest, );
		// newRequest.printResult();
		// std::cout << "============gf=========\n";
		// stringContainer body = newRequest.getBody();
		// stringContainer::iterator begin = body.begin();
		// stringContainer::iterator end = body.end();
		// while (begin != end)
		// {
		// 	std::cout << *begin;
		// 	++begin;
		// }
		// std::cout << "=======response===========\n";
		// if (!newRequest.getCGIStatus())
		// {
		// 	Response	newResponse(newRequest, &conf);
		// 	std::cout << newResponse.getResponse();
		// }
		// else
		// 	std::cout << "this shit need CGI handling\n";
		// std::cout << "===============request==============\n";
	}
	catch(const std::exception& e)
	{
		// std::cerr << e.what() << '\n';
	}
	
	
	// std::cout << "IP address: " << newRequest.getStartLine().IpAdress << std::endl;
	// std::cout << "Port: " << newRequest.getStartLine().Port << std::endl;
	// std::cout << "\n===============response==============\n";
	// for (; end != begin; ++begin)
	// {
	// 	std::cout << *begin;
	// }
	//decide if cgi or not
	// std::string line;
	// for (int i; i < 10; ++i)
	// {
	// 	std::cout << newRequest.getCRLF(line, (char *)"\n") << std::endl;
	// 	std::cout << line << std::endl;
	// }
}