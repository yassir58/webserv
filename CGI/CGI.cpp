#include "CGI.hpp"

CGIHandler::CGIHandler()
{
    std::cout << "Calling the default constructor." << std::endl;
}

CGIHandler::CGIHandler(Location *location, Server *server, Request *request)
{
    this->location = location;
    this->defaultPath = location->getCGIDefault();
    this->server = server;
    this->request = request;
}

CGIHandler::~CGIHandler()
{
    std::cout << "Freeing up all the allocated memory." << std::endl;
}

void    CGIHandler::createEnvList()
{
    mapContainer envList;
    headerFieldList value = this->request->getHeaderFieldlist();

   	headerFieldList::iterator end = value.end();
	headerFieldList::iterator begin = value.begin();

    envList["SERVER_SOFTWARE"] = SERVER_SOFTWARE_VERSION;
    envList["SERVER_PROTOCOL"] = HTTP_PROTOCOL;
    envList["SERVER_PORT"] = int2assci(this->server->getPort());
    envList["SERVER_NAME"] = this->server->getServerName();
    envList["SERVER_ADDR"] = this->server->getHost();
    envList["GATEWAY_INTERFACE"] = CGI_INTERFACE;
    envList["PATH_INFO"] = (this->getFilePath().length() > 0 ? "/" : "") + this->getFilePath();
    envList["SCRIPT_NAME"] = this->getScriptName();
    envList["SCRIPT_FILENAME"] = this->location->getRoot() + this->getScriptName();
    envList["QUERY_STRING"] = this->getQuery();
    envList["REQUEST_METHOD"] = this->request->getMethod();
    envList["REQUEST_URI"] = this->request->getRequestTarget();
    envList["REMOTE_IDENT"] = "";
    envList["PATH_TRANSLATED"] = this->location->getRoot() + this->getFilePath();
    for (; begin != end; ++begin)
	{
        if (begin->key == "content-type" || begin->key == "content-length")
            begin++;
        envList["HTTP_" + toUpperCase(begin->key)] = begin->value;
	}
    if (this->request->getHeaderField("content-type") != NULL)
        envList["CONTENT_TYPE"] = this->request->getHeaderField("content-type")->value;
    if (this->request->getHeaderField("content-length") != NULL)
        envList["CONTENT_LENGTH"] = this->request->getHeaderField("content-length")->value;
    this->envList = envList;
}

const char **CGIHandler::getExecuteArgs()
{
    const char **table;

    table = (const char **)malloc(sizeof(const char *) * 3);
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
    table = (const char **)malloc(sizeof(const char *) * (size + 1));
    if (!table)
        return (NULL);
    for (; begin != end; ++begin)
    {
        table[i] = strdup((begin->first + "=" + begin->second).c_str());
        i++;
    }
    table[i] = NULL;
    return (table);
}

std::string CGIHandler::execute()
{
    const char **convertedList;
    int i;

    i = 0;
    this->createEnvList();
    convertedList = this->convertEnvList();
    while (convertedList[i])
    {
        std::cout << convertedList[i] << std::endl;
        i++;
    }
}

/* @details: in the substr function i started from 1 to remove the backslash from the 
script name file path
*/
std::string CGIHandler::getScriptName()
{
    std::string extension = this->location->getCGIExtension();
    std::string urlExample = this->request->getRequestTarget();
    return (urlExample.substr(1, urlExample.find(("." + extension).c_str()) + extension.length())); 
}

std::string CGIHandler::getQuery()
{
    std::string urlExample = this->request->getRequestTarget();
    stringContainer str;
    stringContainer params;
    int i;

    i = 0;
    if (urlExample.find("?") != std::string::npos)
    {
        str = splitSeparator(urlExample, '?');
        if (str.size() > 1)
        {
            params = splitSeparator(str[1], '&');
            while (i < params.size())
            {
                if (params[i].find("=") == std::string::npos)
                    return (std::string());
                i++;
            }
            return (str[1]);
        }
    }
    return (std::string());
}

std::string CGIHandler::getFilePath()
{
    std::string urlExample = this->request->getRequestTarget();
    std::string queryStrippedURL = splitSeparator(urlExample, '?')[0];
    std::string filePath = queryStrippedURL.erase(0, queryStrippedURL.find(this->getScriptName()) + this->getScriptName().length() + 1);
    return (filePath);
}

std::string    CGIHandler::getOutput()
{
    int fds[2];
    int fd;
    std::string output;
    char **envList;
    char **args;
    pid_t pid;

    args = (char **)this->getExecuteArgs();
    if (pipe(fds) < 0)
    {
        std::cout << "Could not use pipe" << std::endl;
        exit(1);
    }
    pid = fork();
    if (pid == 0) // Redirecting the execve output to the file.
    {
        close(fds[1]);
        dup2(fds[0], STDIN_FILENO);
        fd = open("/tmp/CGI", O_RDWR | O_CREAT | 777);
        if (fd < 0)
        {
            std::cout << "Could not open tmp file" << std::endl;
            exit(1);
        }
        execve(this->defaultPath.c_str(), args, envList);
        dup2(fd, STDOUT_FILENO); // Redirecting the execve output to the file.
        close(fds[0]);
        close(fd);
        exit(0);
    }
    else
    {
        close(fds[0]);
        //? I think i should do some more parsing to request body accoring to the encoding type.
        write(fds[1], convertBody(this->request->getBody()).c_str(), convertBody(this->request->getBody()).length());
        close(fds[1]);
        waitpid(-1, NULL, 0);
    }
}

//URL Example: http://localhost/php-cgi/index.php/tv/home?season=5&episode=62

// PATH_INFO: /tv/home
// QUERY_STRING: season=5&episode=62
// SCRIPT_NAME: /php-cgi/index.php
// REQUEST_URI: /php-cgi/index.php/tv/home?season=5&episode=62
