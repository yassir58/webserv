#include "config.hpp"


int main(int argc, char **argv)
{
    std::string line = "pid/var/log/nginx/nginx.pid;";
    std::vector<std::string> keys;

    keys = split(line);

    // int i = 0;
    // while (i < keys.size())
    // {
    //     std::cout << keys.at(i) << std::endl;
    //     i++;
    // }
    return (0);
}