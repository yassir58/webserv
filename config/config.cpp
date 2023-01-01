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
            i = getClosingIndex(this->configContent, i + 1);
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
    std::vector<std::string> ret;
    int size;

    
    ret = split(config[line]);
    size = ret.size();

    validateDirective(ret, MAIN);
    ret[size - 1][ret[size - 1].length() - 1] != ';' ? ret.pop_back() : ret[size - 1].pop_back();
    if (!this->pid_path.empty())
    {
        std::cout << "Syntax Error: Directive already exits" << std::endl;
        exit(1);
    }
    this->pid_path = ret[1];
    checkPath(ret[1], CREATE_MODE);
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

void    Http::parseHttpContext(std::vector<std::string> & configContent, int index)
{
    int size;
    Server server;
    Server *ptr;
    std::vector<std::string> line;
    
    this->pages = new Pages();

    ptr = NULL;
    size = configContent.size();
    while (index < size)
    {
        line = split(configContent[index]);
        if (line.size() == 1 && line[0] == "}")
            return ;
        if (line.size() > 0 && line[0] == "server" && line[1] == "{")
        {
            ptr = server.parseServer(configContent, index + 1);
            ptr->printServer();
            this->servers.push_back(ptr);
            index = getClosingIndex(configContent, index + 1);
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
        if (line.size() == 1 && line[0] == "}")
            return (server);
        if (line.size() == 3 && line[0] == "location" && line[2] == "{")
        {
            server->locations.push_back(location.parseLocation(configFile, line[1], index + 1));
            index = getClosingIndex(configFile, index + 1);
        }
        else
        {
            // printf("First: %p\n", server);
            this->parseDirective(configFile, server, index);
        }
            // Throw an exception or exit with error code.
        index += 1;
    }
    return (server);
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
    else if (ret.size() == 3 && ret[0] == "listen" || ret.size() == 2 && ret[0] == "listen")
    {
        if (ret.size() == 2 && is_number(ret[1]))
        {
            instance->port = atoi(ret[1].c_str());
            instance->host = "127.0.0.1"; //? This line could be commented, because i can set this by default on object constrution.
        }
        else
        {
            instance->port = 80;
            instance->host = strcmp(ret[1].c_str(), "localhost") ? ret[1] : "127.0.0.1";
        }
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

std::vector<Location *> Server::getLocations()
{
    return (this->locations);
}

Location::Location()
{
    // std::cout << "Default constructor of location called" << std::endl;
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

//TODO: I should add a method will should set the default values for the variables.

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
        if (configFile[index].find("}") != std::string::npos)
        {
            if (line.size() == 1 && line[0] == "}")
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
    {
        std::cout << "Syntax error: invalid directive format" << std::endl;
        exit(1);
    }
}