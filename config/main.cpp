#include "config.hpp"


int main(int argc, char **argv)
{
    std::string path;
    stringContainer config;
    int container_size;
    int i;

    // path = "  12.6 .12   .6";
    // config = splitSeparator(path, '.');
    // container_size = config.size();
    // i = 0;
    // while (i < container_size)
    // {
    //     std::cout << config.at(i) << std::endl;
    //     i++;
    // }

    try 
    {
        Config *file = new Config("./webserv.conf");
        file->parseConfig();
        std::cout << "Server Error Log: " << file->getHttpContext()->getServers()[0]->getErrorLog() << std::endl;
        std::cout << "Server Error Log: " << file->getHttpContext()->getServers()[0]->getHost() << std::endl;
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    return (0);
}
