#include "config.hpp"

Config::Config()
{
    std::cout << "Config constructor called but without any params" << std::endl;
}

Config::Config(std::string path)
{
    this->configContent = read_config_file(path);
    // printContainer(this->configContent);
    check_brackets(this->configContent);
}

Config::~Config()
{
    std::cout << "Config destructor called." << std::endl;
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
        {
            std::cout << "Parsing http context" << std::endl;
            this->globalHttpContext.parseHttpContext(configContent, i + 1);
            i += getClosingIndex(this->configContent, i + 1);
        }
        else
        {
            std::cout << "Parsing a normal directive" << std::endl;
            this->parseDirective(this->configContent, i);
        }
        i++;
    }
}

void    Config::parseDirective(std::vector<std::string> config, int line)
{
    std::vector<std::string> splittedLine;
    
    splittedLine = split(config[line]);
    validateDirective(splittedLine, MAIN);
    if (!this->pid_path.empty())
    {
        std::cout << "Syntax Error: Directive already exits" << std::endl;
        exit(1);
    }
    checkPath(splittedLine[1], CREATE_MODE);
}

Http::Http() 
{
    std::cout << "Http default constructor called" << std::endl;
}

Http::~Http() {
    // Should free up any memeory used by the http context.
    std::cout << "Http destructor called" << std::endl;
}

void    Http::parseHttpContext(std::vector<std::string> & configContent, int index)
{
    int size;
    Server server;
    std::vector<std::string> line;
    this->pages = new Pages();

    size = configContent.size();
    while (index < size)
    {
        line = split(configContent[index]);
        if (line.size() > 0 && line[0] == "server" && line[1] == "{")
        {
            this->servers.push_back(server.parseServer(configContent, index + 1));
            index += getClosingIndex(configContent, index + 1);
        }
        else
        {
            std::cout << "Parsing normal directive" << std::endl;
            this->parseDirective(configContent, index);
        }
        index += 1;
    }
}

void    Http::parseDirective(std::vector<std::string> config, int line)
{
    std::vector<std::string> ret;
    int size;
    
    ret = split(config[line]);
    size = ret.size();
    validateDirective(ret, HTTP);
    ret[size - 1][ret[size - 1].length() - 1] != ';' ? ret.pop_back() : ret[size - 1].pop_back();
    if (ret.size() == 3 && ret[0] == "default_page")
    {
        checkPath(ret[2], CHECK_MODE);
        parse_error_pages(ret, this->pages);
    }
    if (ret.size() == 2 && ret[0] == "send_file")
    {
        // ! Checking for null i can check wheather the directive is duplicated or not.
        if (strcmp(ret[1].c_str(), "on") == 0)
            this->sendFile = true;
        else if (strcmp(ret[1].c_str(), "off") == 0)
            this->sendFile = false;
    }
}

Server::Server()
{
    std::cout << "Called the default constructor of server" << std::endl;
}

Server::~Server()
{
    std::cout << "Called the default destructor of server" << std::endl;
}

Server * Server::parseServer(std::vector<std::string> configFile, int index)
{
    int size;
    Server *server = new Server();
    Location location;
    std::vector<std::string> line;

    size = configFile.size();
    while (index < size)
    {
        line = split(configFile[index]);
        if (line.size() == 3 && line[0] == "location" && line[2] == "{")
        {
            std::cout << "Parsing a location context" << std::endl;
            server->locations.push_back(location.parseLocation(configFile, line[1], index + 1));
            index += getClosingIndex(configFile, index + 1);
        }
        else
        {
            std::cout << "Parsing normal directive" << std::endl;
            this->parseDirective(configFile, server, index);
        }
            // Throw an exception or exit with error code.
        index += 1;
    }
    return server;
}

void    Server::parseDirective(std::vector<std::string> config, Server *instance, int line)
{
    std::vector<std::string> ret;
    int size;
    
    ret = split(config[line]);
    size = ret.size();

    validateDirective(ret, SERVER);
    ret[size - 1][ret[size - 1].length() - 1] != ';' ? ret.pop_back() : ret[size - 1].pop_back();
    if (ret.size() == 2 && ret[0] == "root")
    {
        checkPath(ret[1], CHECK_MODE);
        instance->root = ret[1];
    }
    else if (ret.size() == 2 && ret[0] == "server_name")
        instance->serverName = ret[1];
    else if (ret.size() == 2 && ret[0] == "max_body_size")
        instance->maxBodySize = atoi(ret[1].c_str());
    else if (ret.size() == 3 || ret.size() == 2 && ret[0] == "listen")
    {
        if (ret.size() == 2 && is_number(ret[1]))
        {
            instance->port = atoi(ret[2].c_str());
            instance->host = "127.0.0.1";
        }
        else
            instance->port = 80;
        instance->host = strcmp(ret[1].c_str(), "localhost") ? ret[1] : "127.0.0.1";
    }
    else if (ret.size() == 2 && ret[0] == "error_log")
    {
        checkPath(ret[1], CREATE_MODE);
        if (!instance->errorLog.empty())
        {
            std::cout << "Error Log already exists" << std::endl;
            exit(1);
        }
        instance->errorLog = ret[1];
    }
    else if (ret.size() == 2 && ret[0] == "access_log")
    {
        checkPath(ret[1], CREATE_MODE);
        if (!instance->accessLog.empty())
        {
            std::cout << "access Log already exists" << std::endl;
            exit(1);
        }
        instance->accessLog = ret[1];
    }
    else {
        std::cout << "Invalid directive" << std::endl;
        exit(1);
    } 
}

Location::Location()
{
    std::cout << "Default constructor of location called" << std::endl;
}

Location::~Location()
{
    // Free any allocated memory by the location object
    std::cout << "Default destructor of location called" << std::endl;
}

Location::Location(std::string path)
{
    std::cout << "Setting the location path" << std::endl;
    this->endPoint = path;
}

Location * Location::parseLocation(std::vector<std::string> configFile, std::string path, int index)
{
    Location *location = new Location(path);
    std::vector<std::string> line;
    int size;

    size = configFile.size();
    //TODO: This should be changed to the number of lines that the context contains.
    while (index < size)
    {
        line = split(configFile[index]);
        printContainer(line);
        if (configFile[index].find("}") != std::string::npos)
        {
            if (line[0] == "}")
                return (location);
            else 
            {
                std::cout << "Systanx Error: can't join closing bracket with directive";
                exit(1);
            }
        }
        else
            location->parseDirective(line, location);
        index++;
    }
    return (location);
}

void    Location::parseDirective(std::vector<std::string> line, Location *instance)
{
    validateDirective(line, LOCATION);
    if (line[0] == "send_file" && line.size() == 2)
    {
        if (strcmp(line[1].c_str(), "on") == 0)
            instance->sendFile = true;
        else if (strcmp(line[1].c_str(), "off") == 0)
            instance->sendFile = false;
    }
    else if (line[0] == "root" && line.size() == 2)
    {
        checkPath(line[1], CHECK_MODE);
        instance->root = line[1];
    }
    else if (line[0] == "upload_path" && line.size() == 2)
    {
        checkPath(line[1], CHECK_MODE);
        instance->uploadPath = line[1];
    }
    else 
    {
        std::cout << "Syntax error: invalid directive format" << std::endl;
        exit(1);
    }
}