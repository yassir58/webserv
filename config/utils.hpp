#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>

const char * httpContext[] = {
        "send_file",
        "error_log",
        "access_log",
        NULL
};

const char * serverContext[] = {
    	"listen",
        "server_name",
        "root",
        "max_body_size",
        "error_page",
        NULL
};

const char * locationContext[] = {
        "root",
        "send_file",
        "index",
        "upload_path",
        "allowed_methods",
        "cgi_enable",
        "cgi_default",
        "cgi_extension",
        "directory_listing",
        "redirect",
        NULL
};


#endif