#include "config.hpp"


int main(int argc, char **argv)
{
    std::string path;
    std::vector<std::string> config;
    int container_size;
    int i;

    Config conf("./webserv.conf");
    std::cout << "Before the segfault" << std::endl;
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