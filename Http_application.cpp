#include "ServerInstance.hpp"
#include "config/config.hpp"

Http_application::Http_application ()
{
    serverCount = 1;
    connectionCount = 0;
    logFile.open ("server_log", std::ios_base::app);
    indx = serverCount;

}


Http_application::~Http_application ()
{
    std::cout << "\e[0;31m HTTP APPLICATION CLOSED \e[0m" <<std::endl;
    logFile.close ();
    close (epollInstance);
}

Http_application::Http_application (const Http_application &copy)
{
    serverCount = copy.serverCount;
    connectionCount= copy.connectionCount;
    clientMaxBodySize = copy.clientMaxBodySize;
    indx = serverCount;
}

void Http_application::connectServers (void)
{
    struct epoll_event event;
    std::vector<ServerInstance*>::iterator bg;
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



void Http_application::checkForConnection(void)
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

void Http_application::handleNewConnection (int server_indx)
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

void Http_application::allocateServers (void)
{
    std::vector<Server*>::iterator bg;
    std::vector<Server*> serverConfigs;
    std::vector<int> portList;
    ServerInstance *server;
    int servPort;

    serverConfigs = config->getHttpContext ()->getServers ();
    if (serverConfigs.size () > 0)
    {

        for (bg = serverConfigs.begin ();bg != serverConfigs.end (); bg++)
        {
            servPort = (*(*bg)).getPort ();
            server = new ServerInstance ();
            if (std::find (portList.begin (), portList.end (), servPort) == portList.end ())
            {
                portList.push_back (servPort);    
                server->setServerName ((*(*bg)).getServerName ());
                server->setServerPort ((*(*bg)).getPort ());
                server->setService ((*(*bg)).getPort ());
                serverList.push_back (server);
            }
        }
    }
    else
    {
        server = new ServerInstance ();
        server->setServerName (DEFAULT_SERVER_NAME);
        server->setServerPort (DEFAULT_PORT);
        server->setService (DEFAULT_PORT);
        serverList.push_back (server);
    }
}

void Http_application::handleHttpRequest (int fd)
{
    return_value = recv (fd, buffer, HEADER_MAX, 0);
    if (return_value == 0)
        std::cout << "remote peer closed connection" << std::endl;
    else if (return_value == -1)
        handleError  (READERR);
    else
    {
        std::cout << "\e[0;32m recieved data : \e[0m" << std::endl;
        std::cout << buffer << std::endl;
    }
    // for testing and observability
    return_value = send (fd, HTTP_RESPONSE_EXAMPLE, strlen (HTTP_RESPONSE_EXAMPLE), 0);
    logFile << "\e[0;33mbyte sent : \e[0m" << return_value << " \e[0;33mexpected : \e[0m" <<  strlen (HTTP_RESPONSE_EXAMPLE) << std::endl;
}

int Http_application::getConnectionIndx (void)
{
    return (indx);
}

int Http_application::getConnectionCount (void)
{
    return (this->watchedFds.size());
}

int Http_application::getServerCount (void) const
{
    return (this->serverCount);
}

int Http_application::getConnectionCount (void) const
{
    return (this->connectionCount);
}


void Http_application::handleConfig (int argc , char *argv[])
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

void Http_application::printServerInfo (void)
{
    std::vector<ServerInstance*>::iterator bg;

    for (bg = serverList.begin(); bg != serverList.end (); bg++)
    {
        std::cout << (*(*bg)).getServerPort () << std::endl;
        std::cout << (*(*bg)).getServerName() << std::endl;
    }
}


void Http_application::setupAppResources (void) 
{
    epollInstance = epoll_create (MAX_CONNECT);
    if (epollInstance == -1)
        throw Fatal_error (strerror (errno));
}