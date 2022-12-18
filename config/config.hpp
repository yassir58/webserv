#pragma once
#include <iostream>
#include <regex>
#include <fstream>
#include <istream>

#define MAIN_CONTEXT 0
#define HTTP_CONTEXT 1
#define SERVER_CONTEXT 2
#define LOCATION_CONTEXT 3

class Location 
{
    private:
        std::string endPoint;
        std::string root;
    public:
        Location();
        ~Location();
};

class Server {
    private:
        std::string listen;
        std::string serverName;
        std::string root;
        Location locations[];
    public:
        Server();
        ~Server();
};

class http {
    bool sendFile;
    std::string indexes;
    Server virutalServers[];
};

class Config {
    public:
        Config();
        ~Config();
        void get_config();
    private:
        std::string pid;
};