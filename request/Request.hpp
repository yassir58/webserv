/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 20:24:36 by Ma3ert            #+#    #+#             */
<<<<<<< Updated upstream
/*   Updated: 2023/01/09 18:56:06 by Ma3ert           ###   ########.fr       */
=======
/*   Updated: 2023/01/09 14:03:17 by Ma3ert           ###   ########.fr       */
>>>>>>> Stashed changes
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include "request.h"
# include <sstream>
# define OK			   200
# define CREATED	   201
# define NO_CONTENT	   204
# define BAD_REQUEST   400
# define FORBIDDEN	   403
# define NOT_FOUND	   404
# define NOT_ALLOWED   405
# define NOT_IMPLENTED 501
# define SERVER_ERROR  500
# define HTTP_VERSION  505

<<<<<<< Updated upstream
typedef std::vector<std::string> stringContainer;
typedef std::list<headerField>	headerFieldList;
=======


typedef std::vector<std::string> stringContainer;
>>>>>>> Stashed changes

class Request
{
	private:
		std::string				fileString; // the string that contain the request received
		t_start					startLine; // struct contain all the info in the first line of the request
<<<<<<< Updated upstream
		headerFieldList	headerFields; // linked list store the header fields of the request
=======
		std::list<headerField>	headerFields; // linked list store the header fields of the request
>>>>>>> Stashed changes
		stringContainer			body; // contain the body of the request
		int						statusCode; // the error code or the status code
	public:
		Request(std::string	file); //param constructor take a string as param
		~Request(); // destructor not used 
		int	checkMethod(void); // check if the method is valid
		int	treatAbsoluteURI(void); // treat the case where the request contain an absolute URI
		int	treatAbsolutePath(void); // treat the case where the request contain an absolute path
		int checkRequestTarget(void); // check if the request target is valid
		int	checkVersion(void); // check the HTTP version
		int	checkContentParsed(void); // check the content that been parsed
		int parseFirstLine(std::string line); // parse the first line of the request
<<<<<<< Updated upstream
		int parseHeaderField(headerFieldList &list, std::string line); // parse the header fields and store it in linked list
=======
		int parseHeaderField(std::list<headerField> &list, std::string line); // parse the header fields and store it in linked list
>>>>>>> Stashed changes
		int	parseBody(std::string line); // parse the body and store it in the body(look at the private attribute)
		int	getCRLF(std::string &newLine, char *delim); // split the fileString by delim given as argument and store it on the newLine
		void	printResult(void); // print the result produced
		t_start &getStartLine(void); // geter of the startLine
<<<<<<< Updated upstream
		headerFieldList &getHeaderField(void); // geter of the headerFields linked list
=======
		std::list<headerField> &getHeaderField(void); // geter of the headerFields linked list
>>>>>>> Stashed changes
		void	setFileString(std::string &file); // seter of the file string
		void	setStatusCode(int newStatusCode); // seter of the status code
		std::string		getErrorCode(void); // geterof the error code as string (i think I should make one that return the int repre)
		int				getStatusCode(void);
		std::string		getMethod(void); // geter of the method
		std::string		getHTTPVersion(void); // geter of the HTTP version
		std::string		getRequestTarget(void); // geter of the request target
};
#endif