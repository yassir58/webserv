#ifndef BASIC_CN_SEVER_HPP
#define BASIC_CN_SERVER_HPP


#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h> 
#include <iostream>
#include <sys/select.h>


#define PORT 8080
#define HEADER_MAX 8000
#define BUFFER_SIZE 10
#define BACK_LOG_MAX 5 // * max size permitted by most systems 
#define ACK_MESSAGE "\e[0;33m acknowledgement message \e[0m"

class Fatal_error : public std::exception
{
    const char* what() const throw();
};

class Connection_error : public std::exception
{
    const char* what() const throw();
    const char * err_description;
    public:
        Connection_error (const char *desc);
};


class Server_instance
{
    private:
        std::string _server_name;
        int _server_fd;
        int _connection_fd;
        int _connection_port;
        int _addr_len;
        sockaddr_in _server_addr;
        fd_set _current_fds;
        fd_set _active_fds;
        std::string _request_text;
        int _server_alive;
        int _read_return ;
        char _buffer[HEADER_MAX];
        int err_check;
        int enable;
        Fatal_error f_err;
    public :
        Server_instance (void); // init server with random port number
        Server_instance (int port, std::string name); // init server with the given port
        ~Server_instance ();
        //Server_instance (const Server_instance &copy);
        //Server_instance &Server_instance::operator= (const Server_instance &assign);
        int establish_connection (void);
        void handle_request (void);
        void handle_active_sockets (int socket_fd);
};

class connection_state
{

};

class request
{

};

#endif