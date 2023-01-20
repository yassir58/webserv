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
    // std::cout << "Freeing up all the allocated memory." << std::endl;
}

void    CGIHandler::createEnvList()
{
    mapContainer envList;
    headerFieldList value = this->request->getHeaderFieldlist();

   	headerFieldList::iterator end = value.end();
	headerFieldList::iterator begin = value.begin();

    envList["DOCUMENT_ROOT"] = this->server->getRoot();
    envList["SERVER_SOFTWARE"] = SERVER_SOFTWARE_VERSION;
    envList["SERVER_PROTOCOL"] = HTTP_PROTOCOL;
    envList["SERVER_PORT"] = int2assci(this->server->getPort());
    envList["SERVER_NAME"] = this->server->getServerName();
    envList["SERVER_ADDR"] = this->server->getHost();
    envList["GATEWAY_INTERFACE"] = CGI_INTERFACE;
    envList["PATH_INFO"] = (this->getFilePath().length() > 0 ? "/" : "") + this->getFilePath();
    envList["SCRIPT_NAME"] = this->getScriptName(0);
    envList["SCRIPT_FILENAME"] = this->location->getRoot() + this->getScriptName(1);
    envList["QUERY_STRING"] = this->getQuery();
    envList["REDIRECT_STATUS"] = "200";
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
    envList["CONTENT_LENGTH"] = int2assci(31);
    // envList["CONTENT_LENGTH"] = convertBody(this->request->getBody()).length();
    this->envList = envList;
}

const char **CGIHandler::getExecuteArgs()
{
    const char **table;

    table = (const char **)malloc(sizeof(const char *) * 3);
    if (!table)
        return (NULL);
    table[0] = strdup(this->location->getCGIDefault().c_str());
    table[1] = strdup((this->location->getRoot() + this->getScriptName(1)).c_str());
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

    this->createEnvList();
    this->getOutput();
}

/* @details: in the substr function i started from 1 to remove the backslash from the 
script name file path
*/
//! In this function i should check that the extension is available otherwise return null.
std::string CGIHandler::getScriptName(int status)
{
    std::string extension = this->location->getCGIExtension();
    std::string urlExample = this->request->getRequestTarget();
    if (urlExample.find(("." + extension)) == std::string::npos)
    {
        std::cout << "Error extension does not exists in request error using CGI" << std::endl;
        exit(1);
    }
    return (urlExample.substr(status, (urlExample.find(("." + extension).c_str()) + extension.length()) + !status)); 
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

//! Should check the scriptname function because its not doing any validation.
std::string CGIHandler::getFilePath()
{
    std::string urlExample = this->request->getRequestTarget();
    std::string queryStrippedURL = splitSeparator(urlExample, '?')[0];
    std::string filePath = queryStrippedURL.erase(0, queryStrippedURL.find(this->getScriptName(0)) + this->getScriptName(0).length() + 1);
    return (filePath);
}

std::string    CGIHandler::getOutput()
{
    int fds[2];
    int fd;
    char **envList;
    char **args;
    std::string output;
    pid_t pid;

    args = (char **)this->getExecuteArgs();
    envList = (char **)this->convertEnvList();
    print_table(args);
    if (pipe(fds) < 0)
    {
        std::cout << "Could not use pipe" << std::endl;
        exit(1);
    }
    pid = fork();
    if (pid == 0)
    {
        close(fds[1]);
        fd = open("/home/sn4r7/Desktop/CGI", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (fd < 0)
        {
            std::cout << "Could not open tmp file" << std::endl;
            exit(1);
        }
        dup2(fds[0], STDIN_FILENO); // Reading from the read end of the pipe.
        dup2(fd, STDOUT_FILENO); // Redirecting the execve output to the file.
        dup2(fd, STDERR_FILENO); // Redirecting the execve errors to the file.
        execve(this->defaultPath.c_str(), args, envList);
        close(fd);
        close(fds[0]);
        exit(0);
    }
    else
    {
        stringContainer str;
        str.push_back("username=havel&password=secure");
        close(fds[0]);
        //? I think i should do some more parsing to request body accoring to the encoding type.
        // write(fds[1], convertBody(this->request->getBody()).c_str(), convertBody(this->request->getBody()).length());
        write(fds[1], convertBody(str).c_str(), convertBody(str).length());
        close(fds[1]);
        waitpid(-1, NULL, 0);
    }
    return (std::string());
}

//URL Example: http://localhost/php-cgi/index.php/tv/home?season=5&episode=62

// PATH_INFO: /tv/home
// QUERY_STRING: season=5&episode=62
// SCRIPT_NAME: /php-cgi/index.php
// REQUEST_URI: /php-cgi/index.php/tv/home?season=5&episode=62
