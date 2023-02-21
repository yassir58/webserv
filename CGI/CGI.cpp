#include "CGI.hpp"

CGIHandler::CGIHandler()
{
    std::cout << "Calling the default constructor." << std::endl;
}

CGIHandler::CGIHandler(Request *request)
{
    this->location = request->getLocation();
    this->defaultPath = request->getLocation()->getCGIDefault();
    this->server = request->getServerInstance();
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
    envList["SCRIPT_FILENAME"] = (this->location->getRoot().empty() ? this->server->getRoot() : this->location->getRoot()) + this->getScriptName(1);
    envList["QUERY_STRING"] = this->getQuery();
    envList["REDIRECT_STATUS"] = "200";
    envList["REQUEST_METHOD"] = this->request->getMethod();
    envList["REQUEST_URI"] = this->request->getRequestTarget();
    envList["REMOTE_IDENT"] = "";
    envList["PATH_TRANSLATED"] = (this->location->getRoot().empty() ? this->server->getRoot() : this->location->getRoot()) + this->getFilePath();
    for (; begin != end; ++begin)
	{
        if (begin->key == "content-type" || begin->key == "content-length")
            begin++;
        envList["HTTP_" + toUpperCase(begin->key)] = begin->value;
	}
    if (this->request->getHeaderField("Content-Type") != NULL)
        envList["CONTENT_TYPE"] = this->request->getHeaderField("Content-Type")->value;
    envList["CONTENT_LENGTH"] = int2assci(this->request->getBody().length());
    this->envList = envList;
}

const char **CGIHandler::getExecuteArgs()
{
    const char **table;

    table = (const char **)malloc(sizeof(const char *) * 3);
    if (!table)
        return (NULL);
    table[0] = strdup(this->location->getCGIDefault().c_str());
    if (this->location->getRoot().empty())
        table[1] = strdup((this->server->getRoot() + this->getScriptName(1)).c_str());
    else
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

std::string getStatusLine(std::string headers)
{
    std::string status;

    status = headers.substr(headers.find("Status"), headers.find_first_of('\n', headers.find("Status")));
    return ("HTTP/1.1 " + status.erase(0, 8));
}

std::string CGIHandler::formCGIResponse(std::string headers, std::string body)
{
    std::stringstream response;
    std::stringstream generatedHeaders;
    std::string statusLine;

    statusLine = "HTTP/1.1 200 OK";
    generatedHeaders << generateDate();
    generatedHeaders << "Server: " << SERVER_SOFTWARE_VERSION << "\r\n";
    generatedHeaders << "Content-length: " << int2assci(body.length());
    if (headers.length() > 0)
    {
        if (headers.find("Status") != std::string::npos)
            statusLine = getStatusLine(headers);
        generatedHeaders << "\r\n";
        generatedHeaders << headers;
    }
    response << statusLine << "\r\n";
    response << generatedHeaders.str() << "\r\n\r\n";
    response << body;
    return (response.str());
}

std::string CGIHandler::execute()
{
    std::string result;
    std::string headers = "";
    std::string body = "";
    // This function should be able to format the response and add some http headers.
    this->createEnvList();
    result = this->getOutput();
    if (result.find("\r\n\r\n") != std::string::npos)
    {
        headers = result.substr(0, result.find("\r\n\r\n"));
        body = result.substr(result.find("\r\n\r\n") + 4, result.length());
    }
    else if (result.find("Content-type") != std::string::npos)
        headers = result;
    else
        body = result;
    return(this->formCGIResponse(headers, body)); 
}

/* @details: in the substr function i started from 1 to remove the backslash from the 
script name file path
*/
std::string CGIHandler::getScriptName(int status)
{
    std::string extension = this->location->getCGIExtension();
    std::string urlExample = this->request->getRequestTarget();
    std::string urlLink;
    if (urlExample.find(("." + extension)) == std::string::npos)
    {
        urlLink = urlExample.substr(status, urlExample.length());
        if (urlLink[urlLink.length() - 1] != '/')
            urlLink.append("/");
        return (urlLink + this->request->getLocation()->getDefaultIndex());
        // return (urlExample + this->request->getLocation()->getDefaultIndex());
    }
    return (urlExample.substr(status, (urlExample.find(("." + extension).c_str()) + extension.length()) + !status)); 
}

std::string CGIHandler::getQuery()
{
    std::string urlExample = this->request->getRequestTarget();
    stringContainer str;
    stringContainer params;
    unsigned int i;

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
    int trash;
    char **envList;
    char **args;
    std::string output;
    pid_t pid;

    args = (char **)this->getExecuteArgs();
    envList = (char **)this->convertEnvList();
    if (!args || !envList)
        return (std::string());
    print_table (args);
    print_table (envList);
    if (pipe(fds) < 0)
        throw CGIError("CGI Error: Could not open pipe.");
    pid = fork();
    if (pid == 0)
    {
        close(fds[1]);
        fd = open("/tmp/CGI", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        trash = open("/dev/null", O_WRONLY);
        if (fd < 0 || trash < 0)
            throw CGIError("CGI Error: Invalid fd.");
        if (this->location->getCGIExtension() == "php")
            dup2(trash, STDERR_FILENO);
        else
            dup2(fd, STDERR_FILENO);
        dup2(fds[0], STDIN_FILENO); // Reading from the read end of the pipe.
        dup2(fd, STDOUT_FILENO); // Redirecting the execve output to the file.
        execve(this->defaultPath.c_str(), args, envList);
        close(fd);
        close(fds[0]);
        exit(0);
    }
    else
    {
        close(fds[0]);
        write(fds[1], this->request->getBody().c_str(), this->request->getBody().length());
        close(fds[1]);
        freeTable(args);
        freeTable(envList);
        waitpid(-1, NULL, 0);
    }
    output = readContent("/tmp/CGI");
    return (output);
}

//URL Example: http://localhost/php-cgi/index.php/tv/home?season=5&episode=62

// PATH_INFO: /tv/home
// QUERY_STRING: season=5&episode=62
// SCRIPT_NAME: /php-cgi/index.php
// REQUEST_URI: /php-cgi/index.php/tv/home?season=5&episode=62
