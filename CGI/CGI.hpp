#include <iostream>
#include <exception>
#include "../config/config.hpp"

class Request {
    private:
        std::string host;
        std::string port;
};

class CGIHandler 
{
    private:
        std::string defaultPath;
        Location location;
        Server server;
        Request request;
        Config *configFile;
        // I need the request struct or class here
        stringContainer     parseQuery();
        stringContainer     createEnvList();
    public:
        CGIHandler(Location location, Server server, Request request);
        ~CGIHandler();
        std::string    execute();
};