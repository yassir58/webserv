#include "CGI.hpp"

int main(void)
{
    int i = 0;
    stringContainer container;
    std::string urlExample = "http://localhost/php-cgi/index.php/tv/home?season=5&episode=62";
    container = splitSeparator(urlExample, '/');
    while (i < container.size())
    {
        std::cout << container[i] << std::endl;
        i++;
    }
    return (0);
}