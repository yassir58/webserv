#pragma once
#include <iostream>
#include <regex>
#include <fstream>
#include <istream>
#include <vector>
#include <algorithm>

#define MAIN_CONTEXT 0
#define HTTP_CONTEXT 1
#define SERVER_CONTEXT 2
#define LOCATION_CONTEXT 3

struct errorPages {
    std::string path_not_found;
    std::string path_forbidden;
    std::string path_internal_error;
};

class Location 
{
    private:
        std::string endPoint;
        std::string root;
        std::string uploadPath;
        std::string index;
        bool sendFile;
    public:
        void parseLocation(std::vector<std::string> configFile, int line);
        Location();
        ~Location();
};

class Server {
    private:
        int port;
        int keepAliveTimeout;
        std::string host;
        std::string listen;
        std::string serverName;
        std::string accessLog;
        std::string errorLog;
        std::vector<Location> locations;
    public:
        void    parseServer(std::vector<std::string> configFile, int line);
        Server();
        ~Server();
};

class Http {
    private:
        bool sendFile;
        errorPages pages;
        std::vector<Server> servers;
    public:

        Http();
        ~Http();
};

class Config {
    public:
        Config();
        Config(std::string path);
        ~Config();
        void    parseConfig();
    private:
        std::vector<std::string> configContent;
        std::string pid_path;
        Http httpContext;
};

//.. Util functions

void	validate_extension(const char *path, char *ext);
void    validate_file_content(std::ifstream & configFile);
std::string getLine(std::string &line);
std::vector<std::string>   read_config_file(std::string & path);
void    check_brackets(std::string filename);
std::vector<std::string> split(std::string line);