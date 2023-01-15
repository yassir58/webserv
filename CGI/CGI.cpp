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
    mapContainer envList;
    headerFieldList value = this->request->getHeaderField();

   	headerFieldList::iterator end = value.end();
	headerFieldList::iterator begin = value.begin();

    envList["SERVER_SOFTWARE"] = SERVER_SOFTWARE_VERSION;
    envList["SERVER_PROTOCOL"] = HTTP_PROTOCOL;
    envList["SERVER_PORT"] = int2assci(this->server->getPort());
    envList["SERVER_NAME"] = this->server->getServerName();
    envList["GATEWAY_INTERFACE"] = CGI_INTERFACE;
    envList["PATH_INFO"] = this->getFilePath();
    envList["SCRIPT_NAME"] = this->getScriptName();
    envList["SCRIPT_FILENAME"] = this->location->getRoot() + this->getScriptName();
    envList["QUERY_STRING"] = this->getQuery();
    envList["REQUEST_METHOD"] = this->request->getMethod();
    envList["REQUEST_URI"] = this->request->getRequestTarget();
    envList["REMOTE_IDENT"] = "";
    envList["PATH_TRANSLATED"] = this->location->getRoot() + this->getFilePath();
    for (; begin != end; ++begin)
	{
        envList["HTTP_" + toUpperCase(begin->key)] = begin->value;
	}
    this->envList = envList;
}

const char **CGIHandler::getExecuteArgs()
{
    const char **table;

    table = (char **)malloc(sizeof(char *) * 3);
    if (!table)
        return (NULL);
    table[0] = this->location->getCGIDefault().c_str();
    table[1] = (this->location->getRoot() + this->getScriptName()).c_str();
    table[2] = NULL;
    return (table);
}

const char ** CGIHandler::convertEnvList()
{
    mapContainer::iterator begin;
    mapContainer::iterator end;
    const char **table;
    int size;
    int i;

    i = 0;
    size = this->envList.size();
    begin = this->envList.begin();
    end = this->envList.end();
    table = (char **)malloc(sizeof(char *) * (size + 1));
    if (!table)
        return (NULL);
    for (; begin != end; ++begin)
    {
        table[i] = (begin->first + "=" + begin->second).c_str();
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
