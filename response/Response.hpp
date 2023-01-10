/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/04 14:44:14 by Ma3ert            #+#    #+#             */
<<<<<<< Updated upstream
/*   Updated: 2023/01/09 18:52:12 by Ma3ert           ###   ########.fr       */
=======
/*   Updated: 2023/01/09 12:42:02 by Ma3ert           ###   ########.fr       */
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
	int			code; // status code in the numeric form
	std::string	status; // status code in the character form
=======
	int			code;
	std::string	status;
>>>>>>> Stashed changes
} t_status;

class Response
{
	private:
		Request			*request; // a pointer on the request that have been parsed by the all mighty ma3ert
<<<<<<< Updated upstream
		t_status		status[11]; // table of the available status
		int				statusIndex; // the index of the appropriate code response in the table
=======
		t_status		status[11];
		int				statusIndex;
>>>>>>> Stashed changes
	public:
		stringContainer	responseToSend; // public a attribute because it meant to be send
		Response(Request &request, std::string &responseBody); // param constractor first argument is the request and second is the response that should be included the response
		stringContainer generateHeaderFields(std::string &responseBody); // generate the necessary header fields
		std::string generateStatusLine(); // generate the status line
		void	setRequest(Request *request); // seter of the request(look at the private attribute)
		~Response(); // desectrator not used
<<<<<<< Updated upstream
		int		getStatusCode(void); // return the index of the status code
=======
		int		getStatusCode(void);
>>>>>>> Stashed changes
};

#endif /* ******************************************************** RESPONSE_H */