#include <iostream>
#include <exception>
#include "../config/config.hpp"

class Request {
    private:
        std::string host;
        std::string port;
    public:
        std::string getUrl();
};

#define SERVER_SOFTWARE_VERSION "Webserv/1.0"
#define CGI_INTERFACE "CGI/1.1"
#define HTTP_PROTOCOL "HTTP/1.1"

class CGIHandler 
{
    private:
        std::string defaultPath;
        Location location;
        Server server;
        Request request;
        Config *configFile;
        stringContainer envList;
        std::string         getScriptName();
        std::string         getQuery();
        std::string         getFilePath();
        std::string         getRequestURI();
        void                createEnvList();
        char **             convertEnvList();
    public:
        CGIHandler(Location location, Server server, Request request);
        ~CGIHandler();
        std::string    execute();
};

