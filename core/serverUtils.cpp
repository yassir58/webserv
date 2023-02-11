#include "ServerInstance.hpp"
#include "../response/Response.hpp"

int ServerInstance::accept_connection (void)
{
    int new_connection;
    
    struct sockaddr_storage ConnectionAddr;
    socklen_t ConnectionLen = sizeof (ConnectionAddr);

    new_connection = accept (this->serverSocket, (sockaddr *)&ConnectionAddr, (socklen_t *) &ConnectionLen);
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


int Connection::getConnectionSocket (void) const
{
    return (this->ConnectionSocket);
}

std::vector <int>Connection::getResolversList (void) const
{
    return (this->resolversList);
}

char *Connection::getBuffer (void) 
{
    return (this->httpBuffer);
}

void Connection::emptyBuffer (void) 
{
   free (httpBuffer);
}

void Connection::setStatus (int status)
{
	this->status = status;  
}