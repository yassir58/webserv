#include "Server_instance.hpp"


int Server_instance::accept_connection (void)
{
    int new_connection;
    
    struct sockaddr_storage clientAddr;
    socklen_t clientLen = sizeof (clientAddr);
    new_connection = accept (this->_server_fd, (sockaddr *)&clientAddr, (socklen_t *) &clientLen);
    if (new_connection == -1 )
    {
        handleError (ACCEPTERR);
        throw Connection_error (strerror (errno));
    }
    this->_request_count++;
    return (new_connection);
}

void Server_instance::handle_active_sockets (int i)
{
    if (FD_ISSET (i, &ready_fds))
    {
        std::cout << "disc is ready for reading " << i << std::endl;
        if (i != this->_server_fd)
        {
            if (i != this->_server_fd)
                this->handle_request (i);
        }
        else
        {
            std::cout << "accepting new connection" << std::endl;
            this->accept_connection ();
            std::cout << "connections count " << this->_request_count << std::endl;
        }
    }
}

void Server_instance::handle_request (int i)
{
    this->_read_return = read(i, this->_buffer, HEADER_MAX);
    if (this->_read_return == -1)
        throw Connection_error ("read error");
    if (this->_read_return == 0)
        std::cerr << "\e[0;31m NOTHING TO READ \e[0m" << std::endl;
    std::cout << "\e[0;33m message sent by client :\e[0m" << std::endl;
    std::cout << this->_buffer << std::endl;
    send (i, ACK_MESSAGE, strlen (ACK_MESSAGE), 0);
    FD_CLR (i, &fds);
    close (i);
    bzero (this->_buffer, HEADER_MAX);
}

void Server_instance::setServerName (std::string name)
{
    this->_server_name = name;
}

void Server_instance::setServerPort (int port)
{
    this->_connection_port = port;
}

int Server_instance::getServerPort (void)
{
    return (this->_connection_port);
}
std::string Server_instance::getServerName (void) const
{
    return (this->_server_name);
}

void Connection::setFd (int)
{
    this->fd = fd ;
}

void Connection::initFdSet (int fd)
{
    FD_ZERO (&this->timeOutSet);
    FD_SET (fd, &this->timeOutSet);
}

void Connection::setFdType (int type)
{
    this->fd_type = type ;
}