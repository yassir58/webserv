/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 20:24:36 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/07 10:24:27 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include "request.h"
# include <sstream>

typedef std::vector<std::string> stringContainer;

class Request
{
	private:
		std::string				fileString; // the string that contain the request received
		t_start					startLine; // struct contain all the info in the first line of the request
		std::list<headerField>	headerFields; // linked list store the header fields of the request
		stringContainer			body; // contain the body of the request
		int						errorCode; // the error code or the status code
	public:
		Request(std::string	file); //param constructor take a string as param
		~Request(); // destructor not used 
		int	checkMethod(void); // check if the method is valid
		int	treatAbsoluteURI(void); // treat the case where the request contain an absolute URI
		int	treatAbsolutePath(void); // treat the case where the request contain an absolute path
		int checkRequestTarget(void); // check if the request target is valid
		int	checkVersion(void); // check the HTTP version
		int parseFirstLine(std::string line); // parse the first line of the request
		int parseHeaderField(std::list<headerField> &list, std::string line); // parse the header fields and store it in linked list
		int	parseBody(std::string line); // parse the body and store it in the body(look at the private attribute)
		int	getCRLF(std::string &newLine, char *delim); // split the fileString by delim given as argument and store it on the newLine
		void	printResult(void); // print the result produced
		t_start &getStartLine(void); // geter of the startLine
		std::list<headerField> &getHeaderField(void); // geter of the headerFields linked list
		void	setFileString(std::string &file); // seter of the file string
		std::string		getErrorCode(void); // geterof the error code as string (i think I should make one that return the int repre)
};
#endif