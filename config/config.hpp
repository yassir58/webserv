#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <iostream>
#include <regex>
#include <fstream>
#include <istream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <exception>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>

#define MAIN 0
#define HTTP 1
#define SERVER 2
#define LOCATION 3

#define CREATE_MODE 1
#define DIR_MODE 2
#define CHECK_MODE 0

#define PORT_MAX 65535

#define SERVER_SOFTWARE_VERSION "Webserv/1.0"

typedef std::vector<std::string> stringContainer;
typedef std::map<std::string, std::string> mapContainer;

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
        std::string redirectCode;
        std::string defaultIndex;
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
        std::string getRedirectCode();
        std::string getDefaultIndex();
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
        bool errorPages;
        unsigned int port;
        short maxBodySize;
        std::string host;
        std::string root;
        std::string serverName;
        std::vector<Location *> locations;
    public:
        // Getters
        unsigned int getPort();
        short getMaxBodySize();
        std::string getHost();
        std::string getRoot();
        std::string getServerName();
        bool    getErrorPagesStatus();
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
        void    validateConfig();
        void    printConfig();
        void    parseDirective(stringContainer config, int line);
        Http    *getHttpContext();
        mapContainer getMimeMap();
        std::string getPidPath();
    private:
        stringContainer configContent;
        mapContainer mimeMap;
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

bool    validateExtension(std::string path, std::string ext);
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
std::string toUpperCase(std::string str);
std::string convertBody(stringContainer body);
bool validateCGIExtension(std::string key);
bool validateIndexExtension(std::string filename);
std::string readContent(std::string path);
void    print_table(char **table);
void    freeTable(char **table);
std::string	generateDate(void);
mapContainer createMiMe();
bool checkVectorDuplicate(stringContainer container, std::string key);
std::string generateErrorPage(unsigned int statusCode);

#endif 