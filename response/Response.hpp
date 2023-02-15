/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/04 14:44:14 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/02/15 22:59:28 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <unistd.h>
# include <fcntl.h>
# include <string>
# include <sstream>
# include <map>
# include "../request/Request.hpp"
# include "../config/config.hpp"
# include "../core/ServerInstance.hpp"
# include <ctime>

typedef std::vector<std::string> stringContainer;


typedef	struct status
{
	int			code; // status code in the numeric form
	std::string	status; // status code in the character form
} t_status;

class Response
{
	private:
		Request			*request; // a pointer on the request that have been parsed by the all mighty ma3ert
		int				statusIndex; // the index of the appropriate code response in the table
		std::string		responseBody; // the body of the response 
		stringContainer	responseToSend; // the all mighty response
		Config			*configData; // the data from the configuration file
		bool			errorPagestatus;
		std::map<int, std::string> statusCodeMap;
		t_status		status;
	public:
		Response(Request &request, Config *configFile); // param constractor first argument is the request and here I apply method
		~Response(); // desectrator not used
		int				applyMethod(); // apply the method if it's required
		stringContainer generateHeaderFields(std::string &responseBody); // generate the necessary header fields
		std::string 	generateStatusLine(); // generate the status line
		std::string		generateContentType(void); // generate the Content-Type header field
		int				handleRedirection(void); // method use when there's a redirection
		void			setRequest(Request *request, Config *configData); // seter of the request(look at the private attribute)
		void			getStatusCode(void); // return the index of the status code
		void			setResponseBody(std::string responseBody); // setter for the response body
		std::string		getResponse(void); // the getter for the response and it convert the stringContainer to a string
};

#endif /* ******************************************************** RESPONSE_H */