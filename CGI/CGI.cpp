#include "CGI.hpp"

CGIHandler::CGIHandler(Location location, Server server, Request request)
{
    this->location = location;
    this->defaultPath = location.getCGIDefault();
    this->server = server;
    this->request = request;
}

CGIHandler::~CGIHandler()
{
    std::cout << "Default destructor" << std::endl;
}

void    CGIHandler::createEnvList()
{
    stringContainer envList;
    std::string line;
    char buffer[5];

    line = "SERVER_SOFTWARE=";
    envList.push_back(line.append(SERVER_SOFTWARE_VERSION));
    line = "SERVER_PROTOCOL=";
    envList.push_back(line.append(HTTP_PROTOCOL));
    line = "GATEWAY_INTERFACE=";
    envList.push_back(line.append(CGI_INTERFACE));
    line = "SERVER_PORT=";
    envList.push_back(line.append(int2assci(this->server.getPort())));
    line = "SERVER_NAME=";
    envList.push_back(line.append(this->server.getServerName()));
    this->envList = envList;
}

char ** CGIHandler::convertEnvList()
{
    char **table;
    int size;
    int i;

    i = 0;
    size = this->envList.size();
    table = (char **)malloc(sizeof(char *) * size);
    if (!table)
        return (NULL);
    while (i < size)
    {
        table[i] = strdup(this->envList[i].c_str());
        i++;
    }
    table[i] = NULL;
    return (table);
}

std::string CGIHandler::getScriptName()
{
    int i = 0;
    stringContainer container;
    std::string urlExample = "http://localhost/php-cgi/index.php/tv/home?season=5&episode=62";
    container = splitSeparator(urlExample, '/');
    while (i < container.size())
    {
        std::cout << container[i] << std::endl;
        i++;
    }
}

//Query Example: http://localhost/php-cgi/index.php/tv/home?season=5&episode=62

// Host: localhost
// Path info: /tv/home
// Query string: var1=value1&var2=with%20percent%20encoding
// Script name: /php-cgi/index.php
// Request URI: /php-cgi/index.php/tv/home?season=5&episode=62
