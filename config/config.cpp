#include "config.hpp"

Config::Config()
{
    std::cout << "Please specify the path of the config file." << std::endl;
}

Config::Config(std::string path)
{
    this->configContent = read_config_file(path);
    check_brackets(path);
}