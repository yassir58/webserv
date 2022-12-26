#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
#include "../request/Request.hpp"

class Response
{
	private:

	public:
		Response(Request &request, std::string responseBody);
		Response( Response const & src );
		std::list<headerField> generateHeaderFields();
		std::string generateStatusLine();
		~Response();
		Response &		operator=( Response const & rhs );
};

#endif /* ******************************************************** RESPONSE_H */