#include "ServerInstance.hpp"
#include "../config/config.hpp"
#include "../request/request.h"
#include "../request/Request.hpp"
#include "../response/Response.hpp"

HttpApplication::HttpApplication ()
{
    serverCount = 1;
    connectionCount = 0;
    errorLog.open ("error.log", std::ios_base::app);
	accessLog.open ("access.log", std::ios_base::app);
    indx = serverCount;
	fdMax = 0;
}

HttpApplication::~HttpApplication ()
{
    std::cout << "\e[0;31m HTTP APPLICATION CLOSED \e[0m" <<std::endl;
    accessLog.close ();
	errorLog.close ();
    close (queueIdentifier);
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
	struct kevent change;
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
				{
					Connection *newConnection = new Connection (i);

					newConnection->emptyBuffer ();
					err = newConnection->recieveData ();
					if (err == -1)
					{
						delete newConnection;
						FD_CLR (i, &readFds);
						throw Connection_error (strerror (errno), "RECV");
					}
					else if (err == 0)
					{
						delete newConnection;
						FD_CLR (i, &readFds);
						FD_CLR (i , &errorFds);
						close (i);
					}
					else
					{
						serverBlocks servList = this->config->getHttpContext()->getServers ();
						newConnection->setRequest ();
						newConnection->generateResolversList (servList);
						newConnection->matchRequestHandler (servList);
						connections.push_back (newConnection);
						FD_CLR (i, &readFds);
						FD_SET (i, &writeFds);
					}
				}
			}
			if (FD_ISSET (i, &write))
			{
				Connection *connectionInterface = getConnection (i);
				Server *server;
				Request *request;
				std::string dirListBody = listDirectory ("./testing");
				std::string responseHeader (HTTP_RESPONSE_EXAMPLE);

				responseHeader.append (dirListBody);
				if (connectionInterface != nullptr)
				{
					server = connectionInterface->getServer();
					request = connectionInterface->getRequest ();
					connectionInterface->printfResolvers ();
					server->printServer ();
				}
				err = send (i, responseHeader.c_str (), responseHeader.length (), 0);
				if (err == -1)
				{
					FD_CLR (i, &writeFds);
					FD_CLR (i, &errorFds);
					close (i);
					throw Connection_error (strerror (errno), "SEND");
				}
				else
				{
					FD_CLR (i, &writeFds);
					FD_CLR (i, &errorFds);
					close (i);
				}
			}
		}
	}
}

void HttpApplication::handleNewConnection (int serverFd)
{
    int ConnectionSocket;
    ServerInstance *server;
	struct kevent readEvent ;

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
    Connection ConnectionInfo(fd);
	int serverHandlerIndx = 0;


    ConnectionInfo.emptyBuffer ();
    ConnectionInfo.recieveData ();
	std::cout << "\e[0;36m request : \e[0m" << ConnectionInfo.getBuffer () << std::endl;
	// ConnectionInfo.setRequest ();
	// ConnectionInfo.generateResolversList (this->getServerBlockList ());
	// serverHandlerIndx = ConnectionInfo.matchRequestHandler (this->getServerBlockList()) ;
	// ConnectionInfo.sendResponse ();
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


void HttpApplication::setupAppResources (void) 
{
	queueIdentifier = kqueue ();
    if (queueIdentifier  < 0)
        throw Fatal_error (strerror (errno));
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

Connection *HttpApplication::getConnection (int fd)
{
	connectionPool::iterator it;

	for (it  = connections.begin (); it != connections.end (); it++)
	{
		if ((*it)->getConnectionSocket () == fd)
			return ((*it));
	}
	return (nullptr);
}