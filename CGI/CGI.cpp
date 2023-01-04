#include "CGI.hpp"

CGIHandler::CGIHandler(Location location, Server server, Request request)
{
    this->location = location;
    this->server = server;
    this->request = request;
}

CGIHandler::~CGIHandler()
{
    std::cout << "Default destructor" << std::endl;
}

