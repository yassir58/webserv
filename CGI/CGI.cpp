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

stringContainer CGIHandler::createEnvList()
{
    stringContainer envList;
    std::string line;

    line = "SERVER_SOFTWARE=";
    envList.push_back(line.append(SERVER_SOFTWARE_VERSION));
    line = "SERVER_PROTOCOL=";
    envList.push_back(line.append(HTTP_PROTOCOL));
    line = "GATEWAY_INTERFACE=";
    envList.push_back(line.append(CGI_INTERFACE));
    return (envList);
}

//Query Example: http://localhost/php-cgi/index.php/tv/home?season=5&episode=62

// Host: localhost
// Path info: /tv/home
// Query string: var1=value1&var2=with%20percent%20encoding
// Script name: /php-cgi/index.php
// Request URI: /php-cgi/index.php/tv/home?season=5&episode=62
