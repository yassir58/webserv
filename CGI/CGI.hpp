#include <iostream>
#include <exception>
#include "../config/config.hpp"

class Request {
    private:
        std::string host;
        std::string port;
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
        std::string         getScriptName();
        std::string         getQuery();
        std::string         getFilePath();
        stringContainer     createEnvList();
        char **             convertEnvList();
    public:
        CGIHandler(Location location, Server server, Request request);
        ~CGIHandler();
        std::string    execute();
};

