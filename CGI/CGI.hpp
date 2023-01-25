#include <iostream>
#include <exception>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../config/config.hpp"
#include "../request/Request.hpp"

#define SERVER_SOFTWARE_VERSION "Webserv/1.0"
#define CGI_INTERFACE "CGI/1.1"
#define HTTP_PROTOCOL "HTTP/1.1"

typedef std::map<std::string, std::string> mapContainer;

class CGIHandler 
{
    private:
        std::string defaultPath;
        Location *location;
        Server *server;
        Request *request;
        Config *configFile;
        mapContainer envList;
        std::string         getScriptName(int);
        std::string         getQuery();
        std::string         getFilePath();
        std::string         getRequestURI();
        std::string         getOutput();
        std::string         formCGIResponse(std::string , std::string);
        void                createEnvList();
        const char **             convertEnvList();
        const char **             getExecuteArgs();
    public:
        CGIHandler();
        CGIHandler(Location *location, Server *server, Request *request);
        ~CGIHandler();
        std::string         execute();
};

class CGIError: public std::runtime_error
{
    public:
        CGIError(std::string const& msg):
            std::runtime_error(msg)
        {}
};