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

void    Config::parseConfig()
{
    int size;
    int i;
    std::vector<std::string> line;

    i = 0;
    size = this->configContent.size();
    while (i < size)
    {
        line = split(configContent[i]);
        if (line.size() > 0 && line[0] == "http")
            std::cout << "Parsing http context" << std::endl;
            //parseHttpcontext
        else
            std::cout << "Parsing main context directive" << std::endl;
            //parseDirective(line, MAIN_CONTEXT)
    }
}

void    Config::parseDirective(std::vector<std::string> config, int line)
{
    // This will be responsible for parsing each directive
}