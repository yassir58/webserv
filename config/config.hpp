#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <iostream>
#include <regex>
#include <fstream>
#include <istream>
#include <vector>
#include <algorithm>
#include <exception>
#include <sys/stat.h>

#define MAIN 0
#define HTTP 1
#define SERVER 2
#define LOCATION 3

#define CREATE_MODE 1
#define DIR_MODE 2
#define CHECK_MODE 0

typedef std::vector<std::string> stringContainer;

typedef struct errorPages {
    std::string path_not_found;
    std::string path_forbidden;
    std::string path_internal_error;
} Pages;

class Location 
{
    private:
        std::string endPoint;
        std::string root;
        std::string uploadPath;
        std::string index;
        bool sendFile;
    public:
        Location * parseLocation(stringContainer configFile, std::string path, int index);
        void    parseDirective(stringContainer line, Location *instance);
        void    printLocation();
        Location(std::string path);
        Location();
        ~Location();
};

class Server {
    private:
        short port;
        unsigned int maxBodySize;
        std::string host;
        std::string root;
        std::string serverName;
        std::string accessLog;
        std::string errorLog;
        std::vector<Location *> locations;
    public:
        std::vector<Location *>    getLocations();
        Server *  parseServer(stringContainer configFile, int line);
        void    printServer();
        void    printLocations();
        void    parseDirective(stringContainer config, Server *instance, int line);
        Server();
        ~Server();
};

class Http {
    private:
        bool sendFile;
        std::vector<Server *> servers;
    public:
        Pages *pages;
        void    parseDirective(stringContainer config, int line);
        void    parseHttpContext(stringContainer & configFile, int line);
        void    parseErrorPages(std::string line);
        void    printServers();
        bool    getSendFilestatus();
        std::vector<Server *> getServers();
        Http();
        ~Http();
};

class Config {
    public:
        Config();
        Config(std::string path);
        ~Config();
        void    parseConfig();
        void    printConfig();
        void    parseDirective(stringContainer config, int line);
    private:
        stringContainer configContent;
        std::string pid_path;
        Http globalHttpContext;
    
};

class parseConfig: public std::runtime_error
{
    public:
        parseConfig(std::string const& msg):
            std::runtime_error(msg)
        {}
};

void	validate_extension(const char *path, char *ext);
void    validate_file_content(std::ifstream & configFile);
std::string getLine(std::string &line);
stringContainer   read_config_file(std::string & path);
void    check_brackets(stringContainer configContent);
stringContainer split(std::string line);
void    validateDirective(stringContainer & line, int context);
bool checkDirectiveKey(std::string directiveName,const char **directivesTable);
bool checkValidDirectives(std::string line, int context);
void checkPath(std::string path, int mode);
void parse_error_pages(stringContainer page, Pages * errorPages);
bool is_number(const std::string & s);
int getClosingIndex(stringContainer fileContent, int position);
void    printContainer(stringContainer table);

#endif