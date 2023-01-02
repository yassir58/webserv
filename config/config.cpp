#include "config.hpp"

Config::Config()
{
    // std::cout << "Config constructor called but without any params" << std::endl;
}

Config::Config(std::string path)
{
    this->configContent = read_config_file(path);
    // printContainer(this->configContent);
    check_brackets(this->configContent);
}

Config::~Config()
{
    // std::cout << "Config destructor called." << std::endl;
}

void    Config::printConfig()
{
    std::cout << "====Main context:" << std::endl;
    std::cout << "PID path::: " << this->pid_path << std::endl;
    std::cout << "====Http context:" << std::endl;
    if (this->globalHttpContext.getSendFilestatus())
        std::cout << "File upload::: enabled" << std::endl;
    std::cout << "Error Pages: " << std::endl;
    std::cout << "404::: " << this->globalHttpContext.pages->path_not_found << std::endl;
    std::cout << "403::: " << this->globalHttpContext.pages->path_forbidden << std::endl;
    std::cout << "500::: " << this->globalHttpContext.pages->path_internal_error << std::endl;
    this->globalHttpContext.printServers();
}

void    Config::parseConfig()
{
    int size;
    int i;
    stringContainer line;

    i = 0;
    size = this->configContent.size();
    while (i < size)
    {
        line = split(configContent[i]);
        if (line.size() > 0 && line[0] == "http" && line[1] == "{")
        {
            this->globalHttpContext.parseHttpContext(configContent, i + 1);
            i = getClosingIndex(this->configContent, i + 1);
        }
        else
            this->parseDirective(this->configContent, i);
        i++;
    }
}

void    Config::parseDirective(stringContainer config, int line)
{
    stringContainer str;
    int size;

    
    str = split(config[line]);
    size = str.size();

    validateDirective(str, MAIN);
    str[size - 1][str[size - 1].length() - 1] != ';' ? str.pop_back() : str[size - 1].pop_back();
    if (!this->pid_path.empty())
        throw parseError("Syntax Error: PID path file already is set.");
    this->pid_path = str[1];
    checkPath(str[1], CREATE_MODE);
}

Http::Http() 
{
    // std::cout << "Http default constructor called" << std::endl;
}

Http::~Http() {
    // Should free up any memeory used by the http context.
    // std::cout << "Http destructor called" << std::endl;
}

void    Http::printServers(){
    int i;

    i = 0;
    while (i < this->getServers().size())
    {
        std::cout << "====Server context:" << std::endl;
        this->getServers()[i]->printServer();
        if (this->getServers()[i]->getLocations().size() > 0)
            this->getServers()[i]->printLocations();
        i++;
    }
}

void    Http::parseHttpContext(stringContainer & configContent, int index)
{
    int size;
    Server server;
    stringContainer line;
    
    this->pages = new Pages();

    size = configContent.size();
    while (index < size)
    {
        line = split(configContent[index]);
        if (line.size() == 1 && line[0] == "}")
            return ;
        if (line.size() > 0 && line[0] == "server" && line[1] == "{")
        {
            this->servers.push_back(server.parseServer(configContent, index + 1));
            index = getClosingIndex(configContent, index + 1);
        }
        else
            this->parseDirective(configContent, index);
        index += 1;
    }
}

void    Http::parseDirective(stringContainer config, int line)
{
    stringContainer str;
    int size;
    
    str = split(config[line]);
    size = str.size();
    validateDirective(str, HTTP);
    str[size - 1][str[size - 1].length() - 1] != ';' ? str.pop_back() : str[size - 1].pop_back();
    if (str.size() == 3 && str[0] == "default_page")
    {
        checkPath(str[2], CHECK_MODE);
        parse_error_pages(str, this->pages);
    }
    if (str.size() == 2 && str[0] == "send_file")
    {
        // ! Checking for null i can check wheather the directive is duplicated or not.
        if (strcmp(str[1].c_str(), "on") == 0)
            this->sendFile = true;
        else if (strcmp(str[1].c_str(), "off") == 0)
            this->sendFile = false;
    }
}

std::vector<Server *> Http::getServers()
{
    return (this->servers);
} 

bool Http::getSendFilestatus()
{
    return (this->sendFile);
}

Server::Server()
{
    this->maxBodySize = 0;
    this->port = 80;
    this->host = "127.0.0.1";
    this->serverName = "";
    // std::cout << "Called the default constructor of server" << std::endl;
}

Server::~Server()
{
    // std::cout << "Called the default destructor of server" << std::endl;
}

void    Server::printLocations()
{
    int i;
    
    i = 0;
    while (i < this->locations.size())
    {
        std::cout << "====Location context:" << std::endl;
        this->locations[i]->printLocation();
        i++;
    }
}

void    Server::printServer()
{
    std::cout << "HOST: " << this->host << std::endl;
    std::cout << "PORT: " << this->port << std::endl;
    std::cout << "SERVER NAME: " << this->serverName << std::endl;
    std::cout << "ROOT: " << this->root << std::endl;
    std::cout << "MAX BODY SIZE: " << this->maxBodySize << std::endl;
    std::cout << "ERROR LOG: " << this->errorLog << std::endl;
    std::cout << "ACCESS LOG: " << this->accessLog << std::endl; 
}

Server * Server::parseServer(stringContainer configFile, int index)
{
    int size;
    Server *server = new Server();
    Location location;
    stringContainer line;

    size = configFile.size();
    while (index < size)
    {
        line = split(configFile[index]);
        if (line.size() == 1 && line[0] == "}")
            return (server);
        if (line.size() == 3 && line[0] == "location" && line[2] == "{")
        {
            server->locations.push_back(location.parseLocation(configFile, line[1], index + 1));
            index = getClosingIndex(configFile, index + 1);
        }
        else
            this->parseDirective(configFile, server, index);
        index += 1;
    }
    return (server);
}

void    Server::parseDirective(stringContainer config, Server *instance, int line)
{
    stringContainer str;
    int size;

    str = split(config[line]);
    size = str.size();

    validateDirective(str, SERVER);
    str[size - 1][str[size - 1].length() - 1] != ';' ? str.pop_back() : str[size - 1].pop_back();
    if (str.size() == 2 && str[0] == "root")
    {
        checkPath(str[1], CHECK_MODE);
        instance->root = str[1];
    }
    else if (str.size() == 2 && str[0] == "server_name")
        instance->serverName = str[1];
    else if (str.size() == 2 && str[0] == "max_body_size")
        instance->maxBodySize = atoi(str[1].c_str());
    else if (str.size() == 3 && str[0] == "listen" || str.size() == 2 && str[0] == "listen")
    {
        if (str.size() == 2 && is_number(str[1]))
            instance->port = atoi(str[1].c_str());
        else
        {
            instance->port = 80;
            instance->host = strcmp(str[1].c_str(), "localhost") ? str[1] : "127.0.0.1";
        }
    }
    else if (str.size() == 2 && str[0] == "error_log")
    {
        checkPath(str[1], CREATE_MODE);
        if (!instance->errorLog.empty())
            throw parseError("Config Error: Error log path already set");
        instance->errorLog = str[1];
    }
    else if (str.size() == 2 && str[0] == "access_log")
    {
        checkPath(str[1], CREATE_MODE);
        if (!instance->accessLog.empty())
            throw parseError("Config Error: Access log path already set");
        instance->accessLog = str[1];
    }
    else 
        throw parseError("Syntax Error: Invalid Directive");
}

std::vector<Location *> Server::getLocations()
{
    return (this->locations);
}

Location::Location()
{
    this->endPoint = "";
    this->root = "";
    this->sendFile = false;
    this->uploadPath = "";
}

Location::~Location()
{
    // Free any allocated memory by the location object
    // std::cout << "Default destructor of location called" << std::endl;
}

Location::Location(std::string path)
{
    // std::cout << "Setting the location path" << std::endl;
    this->endPoint = path;
}

void    Location::printLocation()
{
    std::cout << "Location endPoint: " << this->endPoint << std::endl;
    if (!this->root.empty())
        std::cout << "ROOT: " << this->root << std::endl;
    if (!this->uploadPath.empty())
        std::cout << "UPLOAD PATH: " << this->uploadPath << std::endl;
    if (this->sendFile)
        std::cout << "UPLOAD ENABLED" << std::endl;
}

Location * Location::parseLocation(stringContainer configFile, std::string path, int index)
{
    Location *location = new Location(path);
    stringContainer line;
    int size;

    size = configFile.size();
    while (index < size)
    {
        line = split(configFile[index]);
        if (configFile[index].find("}") != std::string::npos)
        {
            if (line.size() == 1 && line[0] == "}")
                return (location);
            else
                throw parseError("Syntax Error: Can't join closing bracket with directive");
        }
        else
            location->parseDirective(line, location);
        index++;
    }
    return (location);
}

void    Location::parseDirective(stringContainer line, Location *instance)
{
    int size;

    size = line.size();
    validateDirective(line, LOCATION);
    line[size - 1][line[size - 1].length() - 1] != ';' ? line.pop_back() : line[size - 1].pop_back();
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
        throw parseError("Syntax Error: Invalid directive");
}