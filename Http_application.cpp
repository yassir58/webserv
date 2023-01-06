#include "Server_instance.hpp"


Http_application::Http_application ()
{
    serverCount = 1;
    connectionCount = 0;
    clientMaxBodySize = POST_LIMIT;
    epollInstance = epoll_create (MAX_CONNECT);
    bzero (buffer, HEADER_MAX);
    logFile.open ("server_log", std::ios_base::app);
    indx = serverCount;

}

Http_application::Http_application (int servers_n)
{
    serverCount = servers_n;
    connectionCount= 0;
    clientMaxBodySize = POST_LIMIT;
    epollInstance = epoll_create (MAX_CONNECT);
    bzero (buffer, HEADER_MAX);
    logFile.open ("server_log", std::ios_base::app);
    indx = serverCount;
}

Http_application::~Http_application ()
{
    std::cout << "\e[0;31m HTTP APPLICATION CLOSED \e[0m" <<std::endl;
    delete [] serverList;
    logFile.close ();
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
    int err;
    for (int i = 0; i <  this->serverCount ; i++)
    {
        this->serverList[i].establish_connection ();
        event.data.fd = this->serverList[i].getSocketFd ();
        event.events = POLLIN | POLLOUT;
        serverFds.push_back (event.data.fd);
        err = epoll_ctl (this->epollInstance, EPOLL_CTL_ADD, this->serverList[i].getSocketFd(), &event);
        if (err == -1)
        {
            handleError (POLLERR);
        }
    }
}

Server_instance *Http_application::getServerList (void) const
{
    return (this->serverList);
}

void Http_application::checkReadySockets (void)
{
    struct epoll_event event;
    int fd;
    int server_indx;
    std::vector<int>::iterator it;

    nfds = epoll_wait (this->epollInstance, this->readySockets, MAX_CONNECT, -1);
    if (nfds == -1)
        handleError (POLLERROR);
    else if (nfds == 0)
        std::cout << "connection timeout" << std::endl;
    else
    {
        for (int i = 0; i < nfds; i++)
        {
            if (this->readySockets[i].events & POLLIN)
            {
                fd = this->readySockets[i].data.fd;
                it = std::find (this->serverFds.begin (), this->serverFds.end (), fd);
                if (it != serverFds.end ())
                {
                    // handle server's socket
                    server_indx = it - this->serverFds.begin ();
                    std::cout << server_indx << std::endl;
                    fd = serverList[server_indx].accept_connection ();
                    event.data.fd = fd;
                    event.events = POLLIN | POLLOUT;
                    errValue = epoll_ctl(this->epollInstance, EPOLL_CTL_ADD, fd, &event);
                    if (errValue == -1)
                        handleError (POLLERR);
                    connectionCount++;
                    serverLog (server_indx);
                }
                else
                {
                    // handle client's socket 
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

// void Http_application::handleNewConnection (int server_indx)
// {
//     fd_pool[indx].fd = serverList[server_indx].accept_connection();
//     fd_pool[indx].events = POLLIN;
//     setConnectionFd (fd_pool[indx].fd, CLIENT);
//     indx++;
//     
//     connectionCount++;
// }

// void Http_application::handleReadyConnection (int indx)
// {
//     if (fd_pool[indx].revents == POLLIN || fd_pool[indx].events == POLLOUT)
//     {
//         if (indx >=this->getServerRangeStart () && indx < this->getServerRangeEnd ())
//             handleNewConnection (indx);
//         else
//         {
//             this->handleHttpRequest (indx);   
//         }
//     }
// }

void Http_application::setServerList (Server_instance *list)
{
    this->serverList = list;
}



void Http_application::allocate_servers (void)
{
    try
    {
        this->serverList = new Server_instance[this->serverCount];
    }
    catch (std::bad_alloc &)
    {
        throw Fatal_error ();
    }

}

void Http_application::initServers (void)
{
    // for testing purpose
    std::string serv_names[4];
    int ports[4];
    
    serv_names[0] = "serv1", ports[0] = 1200;
    serv_names[1] = "serv2", ports[1] = 8080;
    serv_names[2] = "serv3", ports[2] = 5000; 
    serv_names[3] = "serv4", ports[4] =  6446;
    for (int i = 0; i < 4; i++)
    {
        this->serverList[i].setServerName (serv_names[i]);
        this->serverList[i].setServerPort (ports[i]);
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


// void Http_application::setConnectionFd (int fd, int type)
// {
//     Connection newConnection;

//     newConnection.setFd (fd);
//     newConnection.initFdSet (fd);
//     newConnection.setFdType (type);
//     this->watchedFds.push_back (newConnection);
// }



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