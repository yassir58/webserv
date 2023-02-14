#include "config.hpp"


int main(void)
{
    // stringContainer str;
    // int i;

    // i = 0;
    // str.push_back("POST");
    // str.push_back("UPDATE");
    // str.push_back("GET");

    // const char * allowedMethods[] = { "POST", "GET", "DELETE"};
    // while (i < str.size())
    // {
    //     if (keyExistsInTable(str[i].c_str(), allowedMethods))
    //         std::cout << "Key exists: " << str.at(i) << std::endl;
    //     else
    //         std::cout << "Key does not exists: " << str.at(i) << std::endl;
    //     i++;
    // }
    // std::string path;
    // stringContainer config;
    // int container_size;
    // int i;

    // path = "  12.6 .12   .6";
    // config = splitSeparator(path, '.');
    // container_size = config.size();
    // i = 0;
    // while (i < container_size)
    // {
    //     std::cout << config.at(i) << std::endl;
    //     i++;
    // }


    Config *file = new Config("../testing/configs/default.conf");
    try 
    {
        file->parseConfig();
    //     std::cout << "Server [0] Host: " << file->getHttpContext()->getServers()[0]->getHost() << std::endl;
    //     std::cout << "Server [0] Port: " << file->getHttpContext()->getServers()[0]->getPort() << std::endl;
    //     std::cout << "Server [0] Server name: " << file->getHttpContext()->getServers()[0]->getServerName() << std::endl;
    //     std::cout << "Server [0] Root: " << file->getHttpContext()->getServers()[0]->getRoot() << std::endl;
        delete file;
        // std::cout << "Server [1] Host: " << file->getHttpContext()->getServers()[1]->getHost() << std::endl;
        // std::cout << "Server [1] Port: " << file->getHttpContext()->getServers()[1]->getPort() << std::endl;
        // std::cout << "Global access log: " << file->getHttpContext()->getAccessLog() << std::endl;
        // std::cout << "Global error log: " << file->getHttpContext()->getErrorLog() << std::endl;
        // std::cout << "Server [1] 404 not found: " << file->getHttpContext()->getServers()[1]->getErrorPages()->path_not_found << std::endl;
        // std::cout << "Server [1] 403 forbidden: " << file->getHttpContext()->getServers()[1]->getErrorPages()->path_forbidden << std::endl;
        // std::cout << "Server [1] 500 internal server error: " << file->getHttpContext()->getServers()[1]->getErrorPages()->path_internal_error << std::endl;
        // std::cout << "Server [1] Port: " << file->getHttpContext()->getServers()[1]->getServerName() << std::endl;
        // printContainer(file->getHttpContext()->getServers()[0]->getLocations()[0]->getMethods());
        // printContainer(file->getHttpContext()->getServers()[0]->getLocations()[1]->getMethods());
        // std::cout << "Directory listing: server[0] location[0]: " << file->getHttpContext()->getServers()[0]->getLocations()[0]->getListingStatus() << std::endl;
        // std::cout << "Redirect link: " << file->getHttpContext()->getServers()[0]->getLocations()[0]->getRedirectLink() << std::endl;
        // std::cout << "Redirect link: " << file->getHttpContext()->getServers()[0]->getLocations()[1]->getRedirectLink() << std::endl;
    }   
    catch(std::exception &e)
    {
        delete file;
        std::cout << e.what() << std::endl;
    }
    return (0);
}
