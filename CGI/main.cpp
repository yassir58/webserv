#include "CGI.hpp"

int main(void)
{
    int i = 0;
    stringContainer container;
    std::string extension = "php";

    std::string urlExample = "http://localhost/php-cgi/home.php/tv/home?season=5&episode=62";
    container = splitSeparator(urlExample, '/');
    while (i < container.size())
    {
        if (!strcmp(container[i].c_str() + container[i].length() - 4, ("." + extension).c_str()))
            std::cout << "Script Name: " << container[i] << std::endl;
        i++;
    }
    return (0);
}