#include "basic_cn_server.hpp"



void Server_instance::handle_active_sockets (int socket_fd)
{
    if (socket_fd == this->_server_fd)
    {
        this->_connection_fd = accept(this->_server_fd, (sockaddr *)&this->_server_addr, (socklen_t*)&this->_addr_len);
        if (this->_connection_fd == -1)
            throw  Connection_error("ACCEPT ERROR");
        FD_SET (this->_connection_fd, &this->_current_fds);
    }
    else
    {
        handle_request ();
        FD_CLR (socket_fd, &this->_current_fds);
        bzero (this->_buffer, HEADER_MAX);
    }
}


void Server_instance::handle_request (void)
{
    this->_read_return = read(this->_connection_fd, this->_buffer, HEADER_MAX);
    if (this->_read_return == 0)
        std::cerr << "\e[0;31m NOTHING TO READ \e[0m" << std::endl;
    std::cout << "\e[0;33m message sent by client :\e[0m" << std::endl;
    std::cout << this->_buffer << std::endl;
    send (this->_connection_fd, ACK_MESSAGE, strlen (ACK_MESSAGE), 0);
    close (this->_connection_fd);
}