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
        if (line.size() > 0 && line[0] == "http" && line[1] == "{")
            this->httpContext.parseHttpContext(configContent, i + 1);
        else
            this->parseDirective(this->configContent, i);
        i++;
    }
}

void    Config::parseDirective(std::vector<std::string> config, int line)
{
    std::vector<std::string> splittedLine;
    
    splittedLine = split(config[line]);
    checkDirective(splittedLine, MAIN_CONTEXT);
    if (!this->pid_path.empty())
    {
        std::cout << "Syntax Error: Directive already exits" << std::endl;
        exit(1);
    }
    createFile(splittedLine[1], CREATE_MODE);
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
    Server server;
    std::vector<std::string> line;

    i = 0;
    size = configContent.size();
    while (index < size)
    {
        line = split(configContent[index]);
        if (line.size() > 0 && line[0] == "server" && line[1] == "{")
            this->servers.push_back(server.parseServer(configContent, index));
            // i += closingBracket();
            // should create a function that will get the location of the
            // closing bracket to add to the numbers of lines to add.
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
    if (splittedLine.size() == 3 && splittedLine[0] == "default_page")
    {
        checkDirective(splittedLine, HTTP_CONTEXT);
        createFile(splittedLine[2], CHECK_MODE);
        parse_error_pages(splittedLine, *this);
    }
    else if (splittedLine.size() == 2)
    {
        checkDirective(splittedLine, HTTP_CONTEXT);
        if (splittedLine[0] == "send_file")
        {
            if (strcmp(splittedLine[1].c_str(), "on") == 0)
                this->sendFile = true;
            else if (strcmp(splittedLine[1].c_str(), "off") == 0)
                this->sendFile = false;
        }
    }
    else {
        //! Throw an exception
        std::cout << "Invalid directive" << std::endl;
        exit(1);
    } 
}

Server Server::parseServer(std::vector<std::string> configFile, int index)
{
    int size;
    int i;
    Server server;
    Location location;
    std::vector<std::string> line;

    size = configFile.size();
    while (index < size)
    {
        line = split(configFile[index]);
        if (line.size() == 3 && line[0] == "location" && line[2] == "{")
            std::cout << "Parsing Location context" << std::endl;
            // server.locations.push_back();
            // i += closingBracket();
            // should create a function that will get the location of the
            // closing bracket to add to the numbers of lines to add.
        else
            this->parseDirective(configFile, i);
            // Throw an exception or exit with error code.
        index += 1;
    }
}

void    Server::parseDirective(std::vector<std::string> config, int line)
{
    std::vector<std::string> splittedLine;
    
    splittedLine = split(config[line]);
    checkDirective(splittedLine, SERVER_CONTEXT);
    if (splittedLine.size() == 2 && splittedLine[0] == "root")
        createFile(splittedLine[2], CHECK_MODE);
    else if (splittedLine.size() == 2 && splittedLine[0] == "server_name")
        this->serverName = splittedLine[1];
    else if (splittedLine.size() == 2 && splittedLine[0] == "max_body_size")
        this->maxBodySize = atoi(splittedLine[1].c_str());
    else if (splittedLine.size() == 3 && splittedLine[0] == "listen")
    {
        this->port = atoi(splittedLine[2].c_str());
        this->host = strcmp(splittedLine[1].c_str(), "localhost") ? splittedLine[1] : "127.0.0.1";
    }
    else if (splittedLine.size() == 2 && splittedLine[0] == "error_log")
    {
        createFile(splittedLine[1], CREATE_MODE);
        if (!this->errorLog.empty())
        {
            std::cout << "Error Log already exists" << std::endl;
            exit(1);
        }
        this->errorLog = splittedLine[1];
    }
    else if (splittedLine.size() == 2 && splittedLine[0] == "access_log")
    {
        createFile(splittedLine[1], CREATE_MODE);
        if (!this->accessLog.empty())
        {
            std::cout << "access Log already exists" << std::endl;
            exit(1);
        }
        this->accessLog = splittedLine[1];
    }
    else {
        //! Throw an exception
        std::cout << "Invalid directive" << std::endl;
        exit(1);
    } 
}