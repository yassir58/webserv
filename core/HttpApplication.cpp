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

}


HttpApplication::~HttpApplication ()
{
    std::cout << "\e[0;31m HTTP APPLICATION CLOSED \e[0m" <<std::endl;
    logFile.close ();
    close (epollInstance);
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
    struct epoll_event event;
    serverContainer::iterator bg;
    int err;
    
    for (bg = serverList.begin (); bg != serverList.end (); bg++)
    {
        try
        {
            (*bg)->establish_connection ();
            event.data.fd = (*bg)->getSocketFd ();
            event.events = POLLIN | POLLOUT;
            serverFds.push_back (event.data.fd);
            err = epoll_ctl (this->epollInstance, EPOLL_CTL_ADD, (*bg)->getSocketFd(), &event);
            if (err == -1)
                throw Connection_error (strerror(errno), "epoll_ctl");
            serverFds.push_back ((*bg)->getSocketFd ());
            
        }
        catch(const std::exception& e)
        {
            // remove server from list
            std::cout << "Invalid " << (*bg)->getHostName () << std::endl;
            (*bg)->setStatus (0);
            // serverList.erase (bg);
            //std::cerr << e.what() << '\n';
        }
    }
}



void HttpApplication::checkForConnection(void)
{
    int fd;
    std::vector<int>::iterator it;

    nfds = epoll_wait (this->epollInstance, this->readySockets, MAX_CONNECT, -1);
    if (nfds == -1)
        throw Connection_error (strerror (errno), "epoll_wait");
    else
    {
        for (int i = 0; i < nfds; i++)
        {
            fd = this->readySockets[i].data.fd;
            it = std::find (this->serverFds.begin (), this->serverFds.end (), fd);
            if (it != serverFds.end ())
                handleNewConnection (this->readySockets[i].data.fd);
            else
            {
                // handle client's socket
                if (this->readySockets[i].events & POLLOUT)
                {
                    handleHttpRequest (this->readySockets[i].data.fd);
                    errValue  = epoll_ctl (this->epollInstance, EPOLL_CTL_DEL, this->readySockets[i].data.fd,&this->readySockets[i]);
                    if (errValue == -1)
                        handleError (POLLERR);
                    errValue = close (this->readySockets[i].data.fd);
                    if (errValue == -1)
                        handleError (CLOSEERR);
                }
            }

        }
    }
}

void HttpApplication::handleNewConnection (int serverFd)
{
    int clientSocket ;
    struct epoll_event event;
    ServerInstance *server;
    
    server = findServerByFd (serverFd); 
    clientSocket = server->accept_connection ();
    event.data.fd = clientSocket;
    event.events = POLLIN | POLLOUT;
    errValue = epoll_ctl(this->epollInstance, EPOLL_CTL_ADD, clientSocket, &event);
    if (errValue == -1)
       throw Connection_error (strerror (errno), "epoll_ctl");
    connectionCount++;
  //  serverLog (server_indx);
}



void HttpApplication::handleHttpRequest (int fd)
{
    Client clientInfo(fd);


    clientInfo.emptyBuffer ();
    clientInfo.recieveData ();
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
        config = new Config ("./default.conf");
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
    epollInstance = epoll_create (MAX_CONNECT);
    if (epollInstance == -1)
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