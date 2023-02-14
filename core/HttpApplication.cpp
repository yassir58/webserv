#include "ServerInstance.hpp"
#include "../config/config.hpp"
#include "../request/request.h"
#include "../request/Request.hpp"
#include "../response/Response.hpp"

HttpApplication::HttpApplication ()
{
    serverCount = 1;
    connectionCount = 0;
    indx = serverCount;
	fdMax = 0;
}

HttpApplication::~HttpApplication ()
{
    std::cout << "\e[0;31m HTTP APPLICATION CLOSED \e[0m" <<std::endl;
}

HttpApplication::HttpApplication (const HttpApplication &copy)
{
    serverCount = copy.serverCount;
    connectionCount= copy.connectionCount;
    HttpMaxBodySize = copy.HttpMaxBodySize;
    indx = serverCount;
}

void HttpApplication::connectServers (void)
{
    serverContainer::iterator bg;
	int fd;
	int indx = 0;
	
    for (bg = serverList.begin (); bg != serverList.end (); bg++)
    {
        try
        {
            (*bg)->establish_connection ();
			fd = (*bg)->getSocketFd();
			if (fd > fdMax)
				fdMax = fd;
            serverFds.push_back (fd);
			watchedFds.push_back (fd);
        }
        catch(const std::exception& e)
        {
            // remove server from list
            std::cout << "Invalid " << (*bg)->getHostName () << std::endl;
            (*bg)->setStatus (0);
            //serverList.erase (bg);
			// bg--;
            // std::cerr << e.what() << '\n';
        }
		indx++;
    }
}

void HttpApplication::initServerSet (void)
{
	std::vector <int>::iterator it;

	FD_ZERO (&readFds);
	FD_ZERO (&writeFds);
	FD_ZERO (&errorFds);
	for (it = serverFds.begin (); it != serverFds.end (); it++)
	{
		FD_SET ((*it), &readFds);
		FD_SET ((*it), &errorFds);
	}
}

void HttpApplication::checkForConnection (void)
{
	fd_set read, write, error;
	int err;
	char buffer[BUFFER_MAX];

	memset (buffer, 0, BUFFER_MAX);
	read = readFds;
	write = writeFds;
	error = errorFds;
	errValue = select (FD_SETSIZE, &read, &write, &error, NULL);
	if (errValue < 0)
		throw Connection_error(strerror(errno), "SELECT");
	else
	{
		for (int i = 0; i < (fdMax + 1); i++)
		{
			if (FD_ISSET (i , &error))
				std::cout <<  "\e[0;31m SOCKET ERROR \e[0m";
			if (FD_ISSET (i, &read))
			{
				if (isServer (i))
					handleNewConnection (i);
				else
					handleHttpRequest (i);

			}
			if (FD_ISSET (i, &write))
				handleHttpResponse (i);
		}
	}
}

void HttpApplication::handleNewConnection (int serverFd)
{
    int ConnectionSocket;
    ServerInstance *server;
	

    server = findServerByFd (serverFd);
    ConnectionSocket = server->accept_connection ();
	if (ConnectionSocket > fdMax)
		fdMax = ConnectionSocket;
	watchedFds.push_back (ConnectionSocket);
	FD_SET (ConnectionSocket, &readFds);
	FD_SET (ConnectionSocket, &errorFds);
    connectionCount++;
}

void HttpApplication::handleHttpRequest (int fd)
{
   	Connection *newConnection ;
	intContainer::iterator it;
   	int recvReturn = 0;
	int bytes_available = 0;
	int start = 0;
	int length  = 0;


	it = std::find (openConnections.begin (), openConnections.end (), fd);
	if (it != openConnections.end ())
		newConnection = (*getConnection (fd));
	else
	{
		newConnection = new Connection (fd);
		connections.push_back (newConnection);
		openConnections.push_back (fd);
	}
	newConnection->setDataTorRead (bytes_available);
	recvReturn = newConnection->recieveData (&start, &length);
	if (recvReturn == -1)
	{
		FD_CLR (fd, &readFds);
		FD_CLR (fd, &errorFds);
		openConnections.pop_back ();
		throw Connection_error (strerror (errno), "RECV");
	}
	else if (recvReturn == 0)
	{
		FD_CLR (fd, &readFds);
		FD_CLR (fd , &errorFds);
		close (fd);
		openConnections.pop_back ();
	}
	else
	{
		if (newConnection->getBodyRead () < newConnection->getContentLength ())
		{
			newConnection->appendBuffer (start, length);
			newConnection->emptyBuffer ();
		}
		else if (newConnection->getBodyRead () == newConnection->getContentLength() 
			|| newConnection->getUpload () <= 0)
		{
			// std::cout << "Request Headers : " << newConnection->getRequestHeaders () << std::endl;
			newConnection->appendBuffer (start, length);
			newConnection->emptyBuffer ();
			binFile.write (newConnection->getRequestBody().data(), newConnection->getRequestBody().size ());
			serverBlocks servList = this->config->getHttpContext()->getServers ();
			newConnection->generateResolversList (servList);
			newConnection->setServerBlocks (servList);
			newConnection->setConfig (this->getConfig ());
			newConnection->setRequest ();
			FD_CLR (fd, &readFds);
			FD_SET (fd, &writeFds);
			openConnections.pop_back ();
		}
	}
	
}

int HttpApplication::getConnectionIndx (void)
{
    return (indx);
}

int HttpApplication::getServerCount (void) const
{
    return (this->serverCount);
}

int HttpApplication::getConnectionCount (void) const
{
    return (this->connectionCount);
}


void HttpApplication::handleConfig (int argc , char *argv[])
{
    if (argc == 1)
        config = new Config ("./testing/configs/default.conf");
    else if (argc == 2)
        config = new Config (argv[argc - 1]);
    else 
        throw Parse_error ("Invalid number of arguments");
    config->parseConfig();
}


// test

void HttpApplication::printServerInfo (void)
{
    serverContainer::iterator bg;

    for (bg = serverList.begin(); bg != serverList.end (); bg++)
    {
        std::cout << (*(*bg)).getServerPort () << std::endl;
        std::cout << (*(*bg)).getHostName() << std::endl;
    }
}


void HttpApplication::filterServerBlocks (void)
{
    serverBlocks serverBlockList;
    serverBlocks::iterator it;
    ServerInstance *server;

    serverBlockList = config->getHttpContext ()->getServers ();
    for (it = serverBlockList.begin(); it != serverBlockList.end (); it++)
    {
        if (!checkServerExistance (*it))
        {
            server = new ServerInstance ((*it)->getHost(), (*it)->getPort ());
            serverList.push_back (server);
        }
    } 
}

int HttpApplication::checkServerExistance (Server *block)
{
    serverContainer::iterator it;

    for (it = serverList.begin (); it != serverList.end (); it++)
    {
        if (!block->getHost().compare ((*it)->getHostName ()) && block->getPort () == (*it)->getServerPort ())
            return TRUE;
    }
    return (FALSE);
}

serverContainer HttpApplication::getServerList(void) const
{
    return (this->serverList);
}

ServerInstance *HttpApplication::findServerByFd (int serverFd)
{
    serverContainer::iterator it;

    for (it= serverList.begin (); it != serverList.end (); it++)
    {
        if ((*it)->getSocketFd () == serverFd)
            return ((*it));
    }
    return (serverList[0]);
}

serverBlocks HttpApplication::getServerBlockList (void) const
{
	return (this->config->getHttpContext()->getServers());
}

connectionPool::iterator HttpApplication::getConnection (int fd)
{
	connectionPool::iterator it ;

	for (it  = connections.begin (); it != connections.end (); it++)
	{
		if ((*it)->getConnectionSocket () == fd)
		{
			// std::cout << "\e[0;33mconnection found\e[0m" << std::endl;
			return it;
		}
	}
	return (it);
}


void HttpApplication::handleHttpResponse (int fd)
{
	connectionPool::iterator it;
	Server *server;
	Request *request;
	Response *newResponse ;

	Connection *connectionInterface = nullptr;

	
	it = getConnection (fd);
	if (it == connections.end ())
		throw Connection_error ("connection not found", "fin jat sara9osta");
	connectionInterface = (*it);
	connectionInterface->constructResponse ();
	errValue = connectionInterface->sendResponse (fd);
	if (errValue == -1)
	{
		FD_CLR (fd, &writeFds);
		FD_CLR (fd, &errorFds);
		close (fd);
		connections.erase (it);
		throw Connection_error (strerror (errno), "SEND");
	}
	else
	{	
		if (errValue)
		{
			std::cout << "\e[0;31m --------------------------> Bytes sent " << connectionInterface->getBytesSent () << "\e[0m" << std::endl;
			FD_CLR (fd, &writeFds);
			FD_CLR (fd, &errorFds);
			close (fd);
			connections.erase (it);
		}
	}
}
