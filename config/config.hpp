#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <iostream>
#include <regex>
#include <fstream>
#include <istream>
#include <vector>
#include <algorithm>
#include <sys/stat.h>

#define MAIN 0
#define HTTP 1
#define SERVER 2
#define LOCATION 3

#define CREATE_MODE 1
#define DIR_MODE 2
#define CHECK_MODE 0

const char * httpContext[] = {
    	"default_page",
	    "send_file"
};

const char * serverContext[] = {
    	"listen",
        "server_name",
		"root",
		"max_body_size",
        "error_log",
        "access_log"
};

const char * locationContext[] = {
        "root",
        "send_file",
        "index",
        "upload_path"
};

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
        Location parseLocation(std::vector<std::string> configFile, std::string path, int index);
        void    parseDirective(std::vector<std::string> line);
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
        std::vector<Location> locations;
    public:
        Server    parseServer(std::vector<std::string> configFile, int line);
        void    parseDirective(std::vector<std::string> config, int line);
        Server();
        ~Server();
};

class Http {
    private:
        bool sendFile;
        Pages pages;
        std::vector<Server> servers;
    public:
        void    parseDirective(std::vector<std::string> config, int line);
        void    parseHttpContext(std::vector<std::string> & configFile, int line);
        void    parseErrorPages(std::string line);
        Pages getErrorPages();
        Http();
        ~Http();
};

class Config {
    public:
        Config();
        Config(std::string path);
        ~Config();
        void    parseConfig();
        void    parseDirective(std::vector<std::string> config, int line);
    private:
        std::vector<std::string> configContent;
        std::string pid_path;
        Http globalHttpContext;
};

void	validate_extension(const char *path, char *ext);
void    validate_file_content(std::ifstream & configFile);
std::string getLine(std::string &line);
std::vector<std::string>   read_config_file(std::string & path);
void    check_brackets(std::string filename);
std::vector<std::string> split(std::string line);
void    validateDirective(std::vector<std::string> line, int context);
bool checkDirectiveKey(std::string directiveName,const char **directivesTable);
bool checkValidDirectives(std::string line, int context);
void checkPath(std::string path, int mode);
void    parse_error_pages(std::vector<std::string> page, Http & context);
bool is_number(const std::string& s);
int getClosingIndex(std::vector<std::string> fileContent, int position);

#endif