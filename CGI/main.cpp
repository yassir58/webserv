#include "CGI.hpp"

int main(void)
{
    std::string scriptName = "index.php";
    std::string urlExample = "http://localhost/php-cgi/index.php/tv/home?season=5&episode=62";
    std::string queryStrippedURL = splitSeparator(urlExample, '?')[0];
    std::string filePath;
    // this url site willl be the refrence of creating the new url.

    filePath = queryStrippedURL.erase(0, queryStrippedURL.find(scriptName) + scriptName.length());

    std::cout << filePath  << std::endl;
    return (0);
}