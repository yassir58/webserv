#include "basic_cn_server.hpp"



void Server_instance::handle_active_sockets (void)
{
        handle_request ();
        this->_request_count++;
        bzero (this->_buffer, HEADER_MAX);
        std::cout << _request_count << std::endl;
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

void Server_instance::accept_connection (void)
{
    int new_connection;

    new_connection = accept (this->_server_fd, (sockaddr*)&this->_server_addr, (socklen_t *) &this->_addr_len);
    if (new_connection == -1)
        strerror (errno);
    FD_SET (new_connection, &_current_fds);
}