#include <iostream>
#include <exception>
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
        std::string         getScriptName();
        std::string         getQuery();
        std::string         getFilePath();
        std::string         getRequestURI();
        void                createEnvList();
        const char **             convertEnvList();
        const char **             getExecuteArgs();
    public:
        CGIHandler(Location *location, Server *server, Request *request);
        ~CGIHandler();
        std::string    execute();
};

