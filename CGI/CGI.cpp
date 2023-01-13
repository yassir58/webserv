#include "CGI.hpp"

CGIHandler::CGIHandler(Location *location, Server *server, Request *request)
{
    this->location = location;
    this->defaultPath = location->getCGIDefault();
    this->server = server;
    this->request = request;
}

CGIHandler::~CGIHandler()
{
    std::cout << "Default destructor" << std::endl;
}

void    CGIHandler::createEnvList()
{
    std::string line;
    std::string tmp;
    stringContainer envList;
    headerFieldList value = this->request->getHeaderField();

   	headerFieldList::iterator end = value.end();
	headerFieldList::iterator begin = value.begin();
    line = "SERVER_SOFTWARE=";
    envList.push_back(line.append(SERVER_SOFTWARE_VERSION));
    line = "SERVER_PROTOCOL=";
    envList.push_back(line.append(HTTP_PROTOCOL));
    line = "SERVER_PORT=";
    envList.push_back(line.append(int2assci(this->server->getPort())));
    line = "SERVER_NAME=";
    envList.push_back(line.append(this->server->getServerName()));
    line = "GATEWAY_INTERFACE=";
    envList.push_back(line.append(CGI_INTERFACE));
    line = "PATH_INFO=";
    envList.push_back(line.append(this->getFilePath()));
    line = "SCRIPT_NAME=";
    envList.push_back(line.append(this->getScriptName()));
    line = "SCRIPT_FILENAME=";
    envList.push_back(line.append(this->location->getRoot() + this->getScriptName()));
    line = "QUERY_STRING=";
    envList.push_back(line.append(this->getQuery()));
    line = "REQUEST_METHOD=";
    envList.push_back(line.append(this->request->getMethod()));
    line = "REQUEST_URI=";
    envList.push_back(line.append(this->request->getRequestTarget()));
    line = "REMOTE_IDENT=";
    envList.push_back(line.append(""));
    line = "PATH_TRANSLATED=";
    envList.push_back(line.append(this->location->getRoot() + this->getFilePath()));
    for (; begin != end; ++begin)
	{
		envList.push_back("HTTP_" + toUpperCase(begin->key) + "=" + begin->value);
	}
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

/* @details: this function is still in testing mode.*/
std::string CGIHandler::getScriptName()
{
    std::string extension = "php";
    std::string urlExample = "/php-cgi/index.php/tv/home?season=5&episode=62";
    return (urlExample.substr(0, urlExample.find(("." + extension).c_str() + extension.length() + 1)));
}

std::string CGIHandler::getQuery()
{
    std::string urlExample = "http://localhost/php-cgi/index.php/tv/home?season=5&episode=62";
    return (splitSeparator(urlExample, '?')[1]);
}

std::string CGIHandler::getFilePath()
{
    std::string scriptName = "index.php";
    std::string urlExample = "http://localhost/php-cgi/index.php/tv/home?season=5&episode=62";
    std::string queryStrippedURL = splitSeparator(urlExample, '?')[0];
    std::string filePath = queryStrippedURL.erase(0, queryStrippedURL.find(scriptName) + scriptName.length());
    return (filePath);
}


//Query Example: http://localhost/php-cgi/index.php/tv/home?season=5&episode=62

// Host: localhost
// Path info: /tv/home
// Query string: season=5&episode=62
// Script name: /php-cgi/index.php
// Request URI: /php-cgi/index.php/tv/home?season=5&episode=62
