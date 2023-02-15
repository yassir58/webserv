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


void HttpApplication::closeConnection (SOCKET fd, std::string  error)
{
	
	int err = 0;

	
	FD_CLR (fd, &readFds);
	FD_CLR (fd, &errorFds);
	err  = close (fd);
	if (err < 0)
		throw Connection_error (strerror (errno), "CLOSE");
	throw Connection_error (error.c_str (), "RECV FAILURE");
}

void HttpApplication::closeConnection (SOCKET fd)
{
	connectionPool::iterator it;

	it = getConnection (fd);
	FD_CLR (fd, &writeFds);
	FD_CLR (fd, &errorFds);
	close (fd);
	if (it == connections.end ())
		throw Connection_error ("CONNECTION ERROR", "COULD NOT FIND CONNECTION");
	connections.erase (it);
}

void HttpApplication::removeConnection (SOCKET fd)
{
	intContainer::iterator it;

	it = std::find (openConnections.begin (), openConnections.end (), fd);
	if (it != openConnections.end ())
		openConnections.erase (it);
}


void Connection::extractHeaderLength (std::string tmpBuffer, int *start, int *len)
{
	size_t headerLen = tmpBuffer.find (CRLF);
	if (headerLen != std::string::npos)
	{
		headerLength = headerLen  + strlen (CRLF);
		*start = headerLength;
		*len  = dataRecievedLength - headerLength;
		requestHeader = tmpBuffer.substr (0, headerLength);
	}
}

void Connection::extractContentLength (std::string tmpBuffer)
{
	std::string pattern ("Content-Length");
	size_t cn = tmpBuffer.find (pattern);

	if (cn != std::string::npos)
	{
		char tmp[10000];
		int i = cn + pattern.length () + 1;
		int j = 0;
		while (tmpBuffer[i] && tmpBuffer[i] != '\n')
		{
			tmp[j] =tmpBuffer[i];
			i++;
			j++;
		}
		tmp[j] = 0;
		ContentLength = atoi (tmp);
		bodyRead = dataRecievedLength - headerLength;
	}
}

void Connection::extractMethod (std::string tmpBuffer)
{
	size_t method = tmpBuffer.find ("GET");
			
	if (method == std::string::npos)
	{
		method = tmpBuffer.find ("POST");
		if (method == 0)
			upload = 1;
	}
	else if (method == 0)
		upload = 0;
}