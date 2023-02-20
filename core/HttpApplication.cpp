/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpApplication.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelatman <yelatman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:37:46 by yelatman          #+#    #+#             */
/*   Updated: 2023/02/20 19:48:47 by yelatman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerInstance.hpp"
#include "../config/config.hpp"
#include "../request/request.h"
#include "../request/Request.hpp"
#include "../response/Response.hpp"


// * *  ------------------------------------------- CONSTRUCTORS -------------------------------------------  ** //

HttpApplication::HttpApplication ()
{
    serverCount = 1;
    connectionCount = 0;
    indx = serverCount;
	fdMax = 0;
	config = NULL ;
	accessLog.open ("./log/access.log", std::ios::app);
	errorLog.open ("./log/error.log", std::ios::app);
}

HttpApplication::~HttpApplication ()
{
    std::cout << "\e[0;31m HTTP APPLICATION CLOSED \e[0m" <<std::endl;
	deleteConfig();
	deleteServers ();
	deleteConnections ();
	accessLog.close ();
	errorLog.close ();
}

HttpApplication::HttpApplication (const HttpApplication &copy)
{
    serverCount = copy.serverCount;
    connectionCount= copy.connectionCount;
    indx = serverCount;
}


// * *  ------------------------------------------- GETTERS -------------------------------------------  ** //


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

serverBlocks HttpApplication::getServerBlockList (void) const
{
	return (this->config->getHttpContext()->getServers());
}

serverContainer HttpApplication::getServerList(void) const
{
    return (this->serverList);
}

connectionPool::iterator HttpApplication::getConnection (int fd)
{
	connectionPool::iterator it ;

	for (it  = connections.begin (); it != connections.end (); it++)
	{
		if ((*it)->getConnectionSocket () == fd)
			return it;
	}
	return (it);
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
            std::cout << "Invalid " << (*bg)->getHostName () << std::endl;
            (*bg)->setStatus (0);
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
	struct timeval			timeout;
	intContainer::iterator it;
	
	
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	read = readFds;
	write = writeFds;
	error = errorFds;
	errValue = select (FD_SETSIZE, &read, &write, &error, &timeout);
	for (it = openConnections.begin (); it != openConnections.end (); ++it)
	{
		checkConnectionTimeOut ((*it));
	}
	if (errValue < 0)
		throw Fatal_error ("select failed");
	else if (!errValue)
		std::cout << "\e[0;32m waiting for connection ... \e[0m" << std::endl;
	else
	{
		for (int i = 0; i < (fdMax + 1); i++)
		{
			if (FD_ISSET (i , &error))
				throw Connection_error(strerror(errno), "SELECT");
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
	connectionPool::iterator cnIt ;
   	int recvReturn = 0;
	int start = 0;
	int length  = 0;


	it = std::find (openConnections.begin (), openConnections.end (), fd);
	if (it != openConnections.end ())
	{
		cnIt = getConnection (fd) ;
		if (cnIt == connections.end ())
			throw Connection_error ("CONNECTION", "CONNECTION NOT FOUND");
		newConnection = (*cnIt);
	}
	else
	{
		newConnection = new Connection (fd);
		newConnection->setPeerAddress ();
		connections.push_back (newConnection);
		openConnections.push_back (fd);
	}
	recvReturn = newConnection->recieveData (&start, &length);
	if (recvReturn == -1)
	{
		newConnection->connectionLog (this->errorLog, "CONNECTION ERROR", "recv failure");
		closeConnection (fd, "RECV ERROR");
		removeConnection (fd);
	}
	else if (recvReturn == 0)
	{
		FD_CLR (fd, &readFds);
		FD_CLR (fd , &errorFds);
		newConnection->connectionLog (this->errorLog, "CONNECTION ERROR", "connection reset by peer");
		removeConnection (fd);
	}
	else
	{
		if (newConnection->getBodyRead () < newConnection->getContentLength ())
			newConnection->appendBuffer (start, length);
		else if (newConnection->getBodyRead () == newConnection->getContentLength() 
			|| newConnection->getUpload () <= 0)
		{
			newConnection->appendBuffer (start, length);
			serverBlocks servList = this->config->getHttpContext()->getServers ();
			newConnection->generateResolversList (servList);
			newConnection->setServerBlocks (servList);
			newConnection->setConfig (this->getConfig ());
			newConnection->setRequest ();
			FD_CLR (fd, &readFds);
			FD_SET (fd, &writeFds);
			newConnection->connectionLog (this->accessLog, REQUEST);
			removeConnection (fd);
		}
	}	
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


void HttpApplication::handleHttpResponse (int fd)
{
	connectionPool::iterator it;
	Server *server;
	Request *request;
	Response *newResponse ;
	

	Connection *connectionInterface = nullptr;

	
	it = getConnection (fd);
	if (it == connections.end ())
		closeConnection (fd, "COULD NOT FIND CONNECTION");
	connectionInterface = (*it);
	connectionInterface->constructResponse ();
	errValue = connectionInterface->sendResponse (fd);
	if (errValue == -1)
	{
		connectionInterface->connectionLog (this->errorLog, "SEND", strerror (errno));
		FD_CLR (fd, &writeFds);
		FD_CLR (fd, &errorFds);
		close (fd);
		connections.erase (it);
		throw Connection_error (strerror (errno), "SEND");
	}
	else if (errValue)
	{
		connectionInterface->connectionLog (this->accessLog, RESPONSE);
		closeConnection (fd);	
	}
}


void HttpApplication::deleteConfig (void)
{
	if (config)
		delete config;
}

void HttpApplication::closeOpenConnections (void)
{
	intContainer::iterator  it;

	for (it = openConnections.begin () ; it != openConnections.end (); ++it)
	{
		close ((*it));
	}
}

void HttpApplication::deleteServers (void)
{
	serverContainer::iterator it ;


	for (it = serverList.begin (); it != serverList.end (); ++it)
	{
		delete (*it);
	}
}

void HttpApplication::deleteConnections (void)
{
	connectionPool::iterator it ;

	for (it = connections.begin (); it != connections.end (); ++it)
	{
		delete (*it);
	}
}