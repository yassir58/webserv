#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

const char * httpContext[] = {
	    "send_file",
        "error_log",
        "access_log"
};

const char * serverContext[] = {
    	"listen",
        "server_name",
		"root",
		"max_body_size",
        "error_page"
};

const char * locationContext[] = {
        "root",
        "send_file",
        "index",
        "upload_path",
        "allowed_methods",
        "cgi_enable",
        "cgi_default",
        "cgi_extension"
};

#endif