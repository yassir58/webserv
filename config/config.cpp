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
    Http http;
    std::vector<std::string> line;

    i = 0;
    size = this->configContent.size();
    while (i < size)
    {
        line = split(configContent[i]);
        if (line.size() > 0 && line[0] == "http")
            std::cout << "Parsing http context" << std::endl;
            //parseHttpcontext
            //::  this function will return a value that will be added to the total
            //number of lines that was used.
        else
            this->parseDirective(this->configContent, i);
        i++;
    }
    this->httpContext = http;
}

void    Config::parseDirective(std::vector<std::string> config, int line)
{
    std::vector<std::string> splittedLine;
    
    splittedLine = split(config[line]);
    checkDirective(splittedLine, MAIN_CONTEXT);
    if (this->pid_path.empty())
    {
        std::cout << "Syntax Error: Directive already exits" << std::endl;
        exit(1);
    }
    createFile(splittedLine[1], CREATE_MODE);
    // check if that path file exists if not create it.
}

Http::Http() 
{
    std::cout << "Http default constructor called" << std::endl;
}

Http::~Http() {
    // Should free up any memeory used by the http context.
    std::cout << "Http destructor called" << std::endl;
}

int    Http::parseHttpContext(std::vector<std::string> & configContent, int index)
{
    int size;
    int i;
    std::vector<std::string> line;

    i = 0;
    size = configContent.size();
    while (index < size)
    {
        line = split(configContent[index]);
        if (line.size() > 0 && line[0] == "server")
            std::cout << "Parsing Server context" << std::endl;
            //parseHttpcontext
        else
            this->parseDirective(configContent, index);
        index += 1;
        i += 1;
    }
    return (i);
}

void    Http::parseDirective(std::vector<std::string> config, int line)
{
    std::vector<std::string> splittedLine;
    
    splittedLine = split(config[line]);
    if (splittedLine.size() == 3)
    {
        std::cout << "Default pages" << std::endl;
        // means that i should parse the default pages.
        // 
    }
    checkDirective(splittedLine, HTTP_CONTEXT);
    createFile(splittedLine[1], CHECK_MODE);
    // check if that path file exists if not create it.
}
