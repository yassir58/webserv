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
#include <cstring>
#include <sstream>

#define MAIN 0
#define HTTP 1
#define SERVER 2
#define LOCATION 3

#define CREATE_MODE 1
#define DIR_MODE 2
#define CHECK_MODE 0

#define PORT_MAX 65535

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
        std::string cgiDefault;
        std::string cgiExtension;
        std::string redirectLink;
        stringContainer methods;
        bool cgiEnable;
        bool sendFile;
        bool listDirectory;
    public:
        // Getters
        std::string getEndPoint();
        std::string getRoot();
        std::string getUploadPath();
        std::string getCGIDefault();
        std::string getCGIExtension();
        std::string getRedirectLink();
        bool getCGIStatus();
        bool getUploadStatus();
        bool getListingStatus();

        stringContainer getMethods();
        void    parseMethods(stringContainer methods);
        Location * parseLocation(stringContainer configFile, std::string path, int index);
        void    parseDirective(stringContainer line, Location *instance);
        void    printLocation();
        // Constructors
        Location(std::string path);
        Location();
        ~Location();
};

class Server {
    private:
        Pages *pages;
        unsigned int port;
        unsigned int maxBodySize;
        std::string host;
        std::string root;
        std::string serverName;
        std::vector<Location *> locations;
    public:
        // Getters
        unsigned int getPort();
        unsigned int getMaxBodySize();
        std::string getHost();
        std::string getRoot();
        std::string getServerName();
        Pages   *getErrorPages();

        
        std::vector<Location *>    getLocations();
        Server *  parseServer(stringContainer configFile, int line);
        void    printServer();
        void    printLocations();
        void    parseDirective(stringContainer config, Server *instance, int line);
        void    parseErrorPages(stringContainer page);
        // Constructors
        Server();
        ~Server();
};

class Http {
    private:
        bool sendFile;
        std::string accessLog;
        std::string errorLog;
        std::vector<Server *> servers;
    public:
        void    parseDirective(stringContainer config, int line);
        void    parseHttpContext(stringContainer & configFile, int line);
        void    printServers();
        bool    getSendFilestatus();
        std::string getAccessLog();
        std::string getErrorLog();
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
        Http    *getHttpContext();
        std::string getPidPath();
    private:
        stringContainer configContent;
        std::string pid_path;
        Http *mainHttpContext;
    
};

class parseError: public std::runtime_error
{
    public:
        parseError(std::string const& msg):
            std::runtime_error(msg)
        {}
};

void	validateExtension(const char *path, char *ext);
void    checkFileEmpty(std::ifstream & configFile);
std::string getLine(std::string &line);
stringContainer   read_config_file(std::string & path);
void    checkBrackets(stringContainer configContent);
stringContainer split(std::string line);
stringContainer splitSeparator(std::string line, char c);
void    validateDirective(stringContainer & line, int context);
bool keyExistsInTable(std::string key, const char **table);
bool checkValidDirectives(std::string line, int context);
void checkPath(std::string path, int mode);
bool isNumber(const std::string & s);
int getClosingIndex(stringContainer fileContent, unsigned int position);
void    printContainer(stringContainer table);
bool    validateHost(std::string ipAddress);
bool checkSpaces(std::string string);
stringContainer   stripSemiColon(stringContainer str);
std::string int2assci(int nb);

#endif 