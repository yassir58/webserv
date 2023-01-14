#include "ServerInstance.hpp"


int ServerInstance::accept_connection (void)
{
    int new_connection;
    
    struct sockaddr_storage clientAddr;
    socklen_t clientLen = sizeof (clientAddr);

    new_connection = accept (this->serverSocket, (sockaddr *)&clientAddr, (socklen_t *) &clientLen);
    if (new_connection < 0)
        throw Connection_error (strerror (errno), "accept");
    this->requestCount++;
    return (new_connection);
}


void ServerInstance::setServerPort (unsigned int port)
{
    this->connectionPort = port;
}

unsigned int ServerInstance::getServerPort (void)
{
    return (this->connectionPort);
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

int Client::getClientSocket (void) const
{
    return (this->clientSocket);
}

std::vector <int>Client::getResolversList (void) const
{
    return (this->resolversList);
}

char *Client::getBuffer (void) 
{
    return (this->buffer);
}

void Client::emptyBuffer (void) 
{
    memset (buffer, 0, sizeof (buffer));
}

int Client::getServerHandlerIndx (void) const
{
    return (this->serverHandlerIndx);
}
