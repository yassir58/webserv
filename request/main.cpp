/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/13 15:35:19 by Ma3ert            #+#    #+#             */
/*   Updated: 2022/12/25 22:41:24 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.h"
#include "Request.hpp"

int main(int ac, char **av)
{
	char fileRequest[] = {"GET /rfc/rfc2616/ HTTP/1.1\r\nConnection: keep-alive\r\n\r\nsomething"};
	Request newRequest(fileRequest);
	newRequest.printResult();
	// std::string line;
	// for (int i; i < 10; ++i)
	// {
	// 	std::cout << newRequest.getCRLF(line, (char *)"\n") << std::endl;
	// 	std::cout << line << std::endl;
	// }
}