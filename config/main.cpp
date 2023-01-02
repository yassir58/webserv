#include "config.hpp"


int main(int argc, char **argv)
{
    std::string path;
    stringContainer config;
    int container_size;
    int i;

    try 
    {
        Config *file = new Config("./webserv.conf");
        file->parseConfig();
        file->printConfig();
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    // config = read_config_file(path);
    // container_size = config.size();
    // i = 0;
    // while (i < container_size)
    // {
    //     std::cout << config.at(i) << std::endl;
    //     i++;
    // }
    return (0);
}