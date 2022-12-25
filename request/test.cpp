#include "request.h"

int main()
{
	std::string sbardila = {"sbardila"};
	std::cout << sbardila[sbardila.length() - 1] << std::endl;
	std::cout << sbardila.substr(1, sbardila.length() - 1);
}