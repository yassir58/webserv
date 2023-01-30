/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/20 20:24:36 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/30 17:14:43 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include "request.h"
# include <sstream>
# include "../config/config.hpp"

# define OK			   200
# define CREATED	   201
# define NO_CONTENT	   204
# define FOUND		   301
# define BAD_REQUEST   400
# define FORBIDDEN	   403
# define NOT_FOUND	   404
# define NOT_ALLOWED   405
# define TOO_LONG	   414
# define TOO_LARGE     413
# define NOT_IMPLENTED 501
# define SERVER_ERROR  500
# define HTTP_VERSION  505

typedef std::vector<std::string> stringContainer;
typedef std::list<headerField>	headerFieldList;

class Request
{
	private:
		std::string				fileString; // the string that contain the request received
		t_start					startLine; // struct contain all the info in the first line of the request
		headerFieldList			headerFields; // linked list store the header fields of the request
		stringContainer			body; // contain the body of the request
		std::string				path; // the path without the query if there's any
		std::string				fileName; // file name specified in the request target
		int						statusCode; // the error code or the status code
		bool					CGI; // bool to check if the request need a CGI handling or not
		size_t 					pos; // used in getCRLF() method
		size_t 					start; // also used in getCRLF() method
		std::string				root; // the root directive if specified
		Server					*serverInstance; // the server instance that handle the request
		Location				*pathLocation; // the location instance meant for the request target
		std::string				redirectionLink; // the link of the redirection from the config file if there's any
		std::string				redirectCode; // the code of the redirection from the config file
		bool					redirectionStatus; // to check if there's any redirection required
		bool					listingStatus; // to check the status of the listing used in the response
		bool					upload; // to check the status of the upload also used in the response
	public:
		Request(std::string fileString, Server *serverInstance); //param constructor take a string as param and the server instance 
		~Request(); // destructor not used
		int				checkMethod(void); // check if the method is valid
		int				treatAbsoluteURI(void); // treat the case where the request contain an absolute URI
		int				treatAbsolutePath(Location *pathLocation); // treat the case where the request contain an absolute path
		int 			checkRequestTarget(void); // check if the request target is valid
		int				checkVersion(void); // check the HTTP version
		int				checkContentParsed(void); // check the content that been parsed
		int 			parseFirstLine(std::string line); // parse the first line of the request
		int 			parseHeaderField(headerFieldList &list, std::string line); // parse the header fields and store it in linked list
		void			parseHostName(std::string &hostNameValue); // parse the host name and store it on the t_start struct (startLine)
		int				parseBody(std::string line); // parse the body and store it in the body(look at the private attribute)
		int				getCRLF(std::string &newLine, char *delim); // split the fileString by delim given as argument and store it on the newLine
		bool			checkCGI(); // to check if the request need a cgi handling
		Location		*matchLocation(); // return the location instance to handle the path specified on the request target
		bool			checkExtension(Location *pathLocation);
		int				checkDirectory(Location *pathLocation);
		std::string		adjustPath(std::string const &prefix, std::string const &sufix);
		bool			checkUpload(Location *pathLocation);
		void			printResult(void); // print the result produced
		t_start 		&getStartLine(void); // geter of the startLine
		headerFieldList &getHeaderFieldlist(void); // geter of the headerFields linked list
		void			setFileString(std::string &file); // seter of the file string
		void			setServerInstance(Server *server);
		void			setStatusCode(int newStatusCode); // seter of the status code
		headerField		*getHeaderField(std::string key); // return a pointer on headerField struct specified in the key param return NULL if not found
		std::string		getErrorCode(void); // geterof the error code as string (i think I should make one that return the int repre)
		std::string		getMethod(void); // geter of the method
		std::string		getHTTPVersion(void); // geter of the HTTP version
		std::string		getRequestTarget(void); // geter of the request target
		int 			getStatusCode(void); // geter of the status code int repre
		std::string		getBody(void); // return the body within the request 
		stringContainer	getBodyStringContainer(void); // return the body within the request 
		bool			getCGIStatus(void); // return the status of the CGI
		std::string		getPath(void); // retunr the path
		bool			getRedirectionStatus(void);
		std::string		getRedirectionLink(void);
		std::string		getRedirectionCode(void);
		bool			getListingStatus(void);
		bool			getUploadStatus(void);
		Location		*getLocation(void);
};
#endif