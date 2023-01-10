#include "ServerInstance.hpp"
#include "../config/config.hpp"

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
        (*(*bg)).establish_connection ();
        event.data.fd = (*(*bg)).getSocketFd ();
        event.events = POLLIN | POLLOUT;
        serverFds.push_back (event.data.fd);
        err = epoll_ctl (this->epollInstance, EPOLL_CTL_ADD, (*(*bg)).getSocketFd(), &event);
        if (err == -1)
            handleError (POLLERR);
    }
}



void HttpApplication::checkForConnection(void)
{
    int fd;
    int server_indx;
    std::vector<int>::iterator it;

    nfds = epoll_wait (this->epollInstance, this->readySockets, MAX_CONNECT, -1);
    if (nfds == -1)
        throw Connection_error (strerror (errno));
    else
    {
        for (int i = 0; i < nfds; i++)
        {
            if (this->readySockets[i].events & POLLIN 
                || this->readySockets[i].events & POLLOUT)
            {
                fd = this->readySockets[i].data.fd;
                it = std::find (this->serverFds.begin (), this->serverFds.end (), fd);
                if (it != serverFds.end ())
                {
                    server_indx = it - this->serverFds.begin ();
                    handleNewConnection (server_indx);
                }
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
}

void HttpApplication::handleNewConnection (int server_indx)
{
    int clientSocket ;
    struct epoll_event event;
    
    clientSocket = serverList[server_indx]->accept_connection ();
    event.data.fd = clientSocket;
    event.events = POLLIN | POLLOUT;
    errValue = epoll_ctl(this->epollInstance, EPOLL_CTL_ADD, clientSocket, &event);
    if (errValue == -1)
       throw Connection_error (strerror (errno));
    connectionCount++;
    serverLog (server_indx);
}



void HttpApplication::handleHttpRequest (int fd)
{
    char buffer[HEADER_MAX];
    returnValue = recv (fd, buffer, HEADER_MAX, 0);
    if (returnValue == 0)
        std::cout << "remote peer closed connection" << std::endl;
    else if (returnValue == -1)
        handleError  (READERR);
    else
    {
        std::cout << "\e[0;32m recieved data : \e[0m" << std::endl;
        std::cout << buffer << std::endl;
    }
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
    // ServerInstance *server;
    // struct addrinfo hints, *resaddr;

    // memset (&hints, 0, sizeof (hints));
    serverBlockList = config->getHttpContext ()->getServers ();
    // hints.ai_family = AF_INET6; // this need to be modified
    // hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE;
    for (it = serverBlockList.begin(); it != serverBlockList.end (); it++)
    {
        (*it)->printServer ();
        // if (!checkServerExistance (*it))
        // {
        //     // check host valid
        //     server = new ServerInstance ((*it)->getHost(), (*it)->getPort ());
        //     errValue = getaddrinfo (server->getHostName ().c_str (), server->getService ().c_str(), &hints, &resaddr);
        //     if (errValue == 0)
        //     {
        //         serverList.push_back (server);
        //     }
        //     else
        //         gai_strerror (errValue);
        // }
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