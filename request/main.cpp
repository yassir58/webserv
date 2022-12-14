/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/13 15:35:19 by Ma3ert            #+#    #+#             */
/*   Updated: 2022/12/14 16:33:59 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.h"

int	treatFirstLine(std::string file, t_start &startline)
{
	std::string sub;
	sub = strtok(file, " ");
}

int main(int ac, char **av)
{
	std::ifstream fileRequest("request.txt");
	std::string line;
	std::string firstLine;
	t_request	newRequest;

	if (!fileRequest.is_open())
		return 1;
	getline(fileRequest, firstLine);
	if (!treatFirstLine(firstLine, newRequest.startLine))
		return 1;
	while (line.empty())
	{
		getline(fileRequest, line);
		treatHeaderField();
	}
	if (line.c_str() && newRequest.getMethod() == "POST")
	{
		std::ofstream body;
		getline(fileRequest, line);
		if (line.c_str())
			body << line;
	}
}