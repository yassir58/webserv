#pragma once
#include <iostream>
#include <regex>
#include <fstream>
#include <istream>

#define MAIN_CONTEXT 0
#define HTTP_CONTEXT 1
#define SERVER_CONTEXT 2
#define LOCATION_CONTEXT 3


class Config {
    public:
        Config();
        ~Config();
        void get_config();
    private:
        std::string pid;
};