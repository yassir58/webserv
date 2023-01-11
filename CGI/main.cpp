#include "CGI.hpp"

int main(void)
{
    int i = 0;
    stringContainer container;
    std::string extension = "php";

    std::string urlExample = "http://localhost/php-cgi/index.php/tv/home?season=5&episode=62";
    std::cout << splitSeparator(urlExample, '?')[1] << std::endl;
   
    return (0);
}