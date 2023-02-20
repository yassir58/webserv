/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 09:31:53 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/02/20 09:36:03 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

int main()
{
	Request newRequest(NOT_FOUND);
	Config conf("../testing/configs/test2.conf");
	conf.parseConfig();	
	Response newResponse(newRequest, &conf);
	std::cout << "===================respone header==================\n" << newResponse.getResponseHeader() << std::endl;
	std::cout << "===================respone body==================\n" << newResponse.getResponseBody().data() << std::endl;
	
}