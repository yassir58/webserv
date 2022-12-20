/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 20:24:36 by Ma3ert            #+#    #+#             */
/*   Updated: 2022/12/20 21:42:18 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include "request.h"

class Request
{
	private:
		t_start						startLine;
		std::list<headerField>		headerFields;
		std::vector<std::string>	body;
		int							errorCode;
	public:
		Request(std::ifstream &file);
		~Request();
		int	checkMethod(void);
		int	treatAbsoluteURI(void);
		int	treatAbsolutePath(void);
		int checkRequestTarget(void);
		int	checkVersion(void);
		int parseFirstLine(std::string line);
		int parseHeaderField(std::list<headerField> &list, std::string line);
		int	parseBody(std::string line);
		void	printResult(void);
		t_start &getStartLine(void);
		std::list<headerField> &getHeaderField(void);

};

#endif