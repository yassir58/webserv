#include "request.h"

int main()
{
	std::string sbardila = {"sbardila"};
	std::string subsbrdl = sbardila.substr(sbardila.length(), std::string::npos);
	std::cout << "your sbardila: " << sbardila << "your sub: " << subsbrdl << std::endl;
}