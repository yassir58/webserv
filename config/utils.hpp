#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

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

#endif