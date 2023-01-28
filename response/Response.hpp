/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/04 14:44:14 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/01/27 20:47:49 by Ma3ert           ###   ########.fr       */
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
		t_status		status[12]; // table of the available status
		int				statusIndex; // the index of the appropriate code response in the table
		std::string		responseBody; // the body of the response 
		stringContainer	responseToSend; // the all mighty response
	public:
		Response(Request &request); // param constractor first argument is the request and here I apply method
		Response(Request &request, std::string CGIOutput); // param constractor first argument is the request and second is the CGI output that should be included the response body
		~Response(); // desectrator not used
		int				applyMethod();
		stringContainer generateHeaderFields(std::string &responseBody); // generate the necessary header fields
		std::string 	generateStatusLine(); // generate the status line
		int				handleRedirection(void);
		void			setRequest(Request *request); // seter of the request(look at the private attribute)
		int				getStatusCode(void); // return the index of the status code
		void			setResponseBody(std::string responseBody);
		std::string		getResponse(void);
};

#endif /* ******************************************************** RESPONSE_H */