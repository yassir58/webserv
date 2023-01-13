#include "CGI.hpp"

int main(void)
{
    int i = 0;
    stringContainer container;
    std::string extension = "php";
    std::string urlExample = "/php-cgi/index.php/tv/home?season=5&episode=62";
    int position = urlExample.find(("." + extension).c_str());
    std::cout << urlExample.substr(0, position + extension.length() + 1)  << std::endl;
    return (0);
}