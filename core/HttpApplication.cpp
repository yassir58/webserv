#include "ServerInstance.hpp"
#include "../config/config.hpp"
#include "../request/request.h"
#include "../request/Request.hpp"

HttpApplication::HttpApplication ()
{
    serverCount = 1;
    connectionCount = 0;
    logFile.open ("server_log", std::ios_base::app);
    indx = serverCount;
	eventIndx = 0;
}

HttpApplication::~HttpApplication ()
{
    std::cout << "\e[0;31m HTTP APPLICATION CLOSED \e[0m" <<std::endl;
    logFile.close ();
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
	
    for (bg = serverList.begin (); bg != serverList.end (); bg++)
    {
        try
        {
            (*bg)->establish_connection ();
			fd = (*bg)->getSocketFd();
            serverFds.push_back (fd);
            EV_SET (&change, fd, EVFILT_READ , EV_ADD | EV_CLEAR , 0, 0, 0);
			errValue = kevent (this->queueIdentifier, &change, 1, NULL, 0, NULL);
			if (errValue < 0)
				throw Connection_error (strerror(errno), "kevent");
			eventIndx++;
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
    }
	
}

void HttpApplication::checkForConnection(void)
{
    int fd;
    std::vector<int>::iterator it;
	struct kevent resultEvents[MAX_CONNECT];

    nfds = kevent(this->queueIdentifier, NULL, 0, resultEvents, MAX_CONNECT, NULL);
    if (nfds < 0)
        throw Connection_error (strerror (errno), "kevent");
    else
    {
        for (int i = 0; i < nfds; i++)
        {
            fd = resultEvents[i].ident;
			if (resultEvents[i].flags & EV_ERROR)
				throw Connection_error (strerror (errno), "EVENT_ERROR");
            else if (resultEvents[i].filter == EVFILT_READ)
            {
            	if (!isServer(fd))
				{
            		handleHttpRequest (fd);
            		errValue = close (fd);
            		if (errValue == -1)
                		handleError (CLOSEERR);
					eventIndx--;
				}
				else
					handleNewConnection (fd);
            }
        }
    }
}

void HttpApplication::handleNewConnection (int serverFd)
{
    int clientSocket;
    ServerInstance *server;
	struct kevent change ;

    server = findServerByFd (serverFd); 
    clientSocket = server->accept_connection ();
	EV_SET (&change, clientSocket, EVFILT_READ , EV_ADD | EV_CLEAR, 0, 0, 0);
	errValue = kevent (this->queueIdentifier, &change, 1, NULL, 0, NULL);
	if (errValue < 0)
		throw Connection_error (strerror (errno), "kevent");
	EV_SET (&change, clientSocket, EVFILT_WRITE , EV_ADD | EV_CLEAR, 0, 0, 0);
	errValue = kevent (this->queueIdentifier, &change, 1, NULL, 0, NULL);
	if (errValue < 0)
		throw Connection_error (strerror (errno), "kevent");
    connectionCount++;
	eventIndx++;
   	//serverLog (server_indx);
}

void HttpApplication::handleHttpRequest (int fd)
{
    Client clientInfo(fd);


    clientInfo.emptyBuffer ();
    clientInfo.recieveData ();
	clientInfo.setRequest ();
	clientInfo.generateResolversList (this->getServerBlockList ());
	clientInfo.printfResolvers ();
	//clientInfo.matchRequestHandler (this->getServerBlockList());
    // for testing and observability
    returnValue = send (fd, HTTP_RESPONSE_EXAMPLE, strlen (HTTP_RESPONSE_EXAMPLE), 0);
    logFile << "\e[0;33mbyte sent : \e[0m" << returnValue << " \e[0;33mexpected : \e[0m" <<  strlen (HTTP_RESPONSE_EXAMPLE) << std::endl;
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
    // epollInstance = epoll_create (MAX_CONNECT);
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
            //(*it)->printServer ();
            // check host valid
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