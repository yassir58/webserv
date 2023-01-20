/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/13 15:35:19 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/20 16:24:03 by Ma3ert           ###   ########.fr       */
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
    std::string url = "/Users/yait-iaz/Desktop/webserv/request/sbardila"; // a rad lquery
    std::string headers = "Content-Type: application/json\r\nAccept: application/json\r\nHost: 192.120.18.0:80\r\nConnection: keep-alive\r\naccept: /\r\naccept-encoding: gzip, deflate, br\r\ncontent-type: multipart/form-data\r\naccept-language: en-US,en;q=0.9\r\nsec-fetch-mode: cors\r\n";
    std::string body = "{\"param1\":\"value1\",\"param2\":\"value2\"}\n";
    std::string body1 = "{\"param1\":\"value1\",\"param2\":\"value2\"}\n";
    std::string body2 = "username=havel\npassword=sbardila\njwt=34D3F564we3f16e16F651e32164843216482";

    std::stringstream request;
    request << method << " " << url << " HTTP/1.1\r\n";
    request << headers;
    request << "Content-Length: " << body.length() << "\r\n";
    request << "\r\n";
    request << body;
    request << body1;
    request << body2;

    std::string request_str = request.str();
	return (request_str);
}

int main(void)
{
	try
	{
		// char fileRequest[] = {	"POST /echo/post/json HTTP/1.1\r\nHost: reqbin.com\r\nAccept: application/json\r\nContent-Type: application/json\r\nContent-Length: 81\r\n\r\n{\nId: 78912,\nCustomer: Jason Sweet,\nQuantity: 1,\nPrice: 18.00\n}\n"};
		// char fileRequest[] = {"POST index.php/tv/home?season=5&episode=62 HTTP/1.1\r\n\r\n"};
		std::string fileRequest = requestGeneratorByGPT();
		Request newRequest(fileRequest);
		std::cout << "this is the raw request: \n";
		std::cout << fileRequest << std::endl;
		std::cout << "\n========================\n";
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
		std::cout << "=======response===========\n";
		Response	newResponse(newRequest, "whatever what the cgi return hoho teyoo jotaro");
		std::cout << newResponse.getResponse();
		// std::cout << "===============request==============\n";
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
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