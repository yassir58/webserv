#include <iostream>
#include <exception>
#include "../config/config.hpp"

class CGIHandler 
{
    private:
        std::string defaultPath;
        Config *configFile;
        // I need the request struct or class here
        stringContainer    parseQuery();
        void    parseEnvList();
    public:
        CGIHandler();
        ~CGIHandler();
        std::string    execute();
};