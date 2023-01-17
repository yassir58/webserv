/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelatman <yelatman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/04 14:44:14 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/14 20:03:17 by yelatman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <unistd.h>
# include <fcntl.h>
# include <string>
# include <sstream>
# include "../request/Request.hpp"
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
		t_status		status[11]; // table of the available status
		int				statusIndex; // the index of the appropriate code response in the table
		std::string		responseBody; // the body of the response 
		stringContainer	responseToSend; // the all mighty response
	public:
		Response(Request &request); // param constractor first argument is the request and second is the response that should be included the response
		~Response(); // desectrator not used
		int				applyMethod();
		stringContainer generateHeaderFields(std::string &responseBody); // generate the necessary header fields
		std::string 	generateStatusLine(); // generate the status line
		void			setRequest(Request *request); // seter of the request(look at the private attribute)
		int				getStatusCode(void); // return the index of the status code
		stringContainer	getResponse(void);
};

#endif /* ******************************************************** RESPONSE_H */