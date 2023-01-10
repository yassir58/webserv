#include "ServerInstance.hpp"


int ServerInstance::accept_connection (void)
{
    int new_connection;
    
    struct sockaddr_storage clientAddr;
    socklen_t clientLen = sizeof (clientAddr);

    new_connection = accept (this->_server_fd, (sockaddr *)&clientAddr, (socklen_t *) &clientLen);
    if (new_connection == -1 )
        throw Connection_error (strerror (errno));
    this->_request_count++;
    return (new_connection);
}

void ServerInstance::setServerName (std::string name)
{
    this->_server_name = name;
}

void ServerInstance::setServerPort (int port)
{
    this->_connection_port = port;
}

int ServerInstance::getServerPort (void)
{
    return (this->_connection_port);
}
std::string ServerInstance::getServerName (void) const
{
    return (this->_server_name);
}

void Connection::setFd (int)
{
    this->fd = fd ;
}

void Connection::initFdSet (int fd)
{
    FD_ZERO (&this->timeOutSet);
    FD_SET (fd, &this->timeOutSet);
}

void Connection::setFdType (int type)
{
    this->fd_type = type ;
}