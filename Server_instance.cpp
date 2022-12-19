#include "Server_instance.hpp"


Server_instance::Server_instance (void)
{
    this->_server_alive = 1;
    this->enable = 1;
    this->_request_count = 0;
    this->_connection_port = 0;
}


Server_instance::~Server_instance (void)
{
    std::cout << "\e[0;31m shutdown server \e[0m" << std::endl;
    close (this->_server_fd);
}

Server_instance::Server_instance (const Server_instance &copy)
{
    this->_server_name = copy._server_name;
    this->_connection_port = copy._connection_port;
}

Server_instance &Server_instance::operator= (const Server_instance &assign)
{
     this->_server_name = assign._server_name;
    this->_connection_port = assign._connection_port;
    return (*this);
}

Server_instance::Server_instance (int port, std::string name)
{
    this->_connection_port = port;
    this->_server_alive = 1;
    this->_request_count = 0;
    this->_server_name = name;
    this->enable = 1;
}

int Server_instance::establish_connection (void)
{
    this->bind_socket ();
    err_check = listen (this->_server_fd, BACK_LOG_MAX);     
    if (err_check == -1)
        throw Connection_error (strerror (errno));
    else
        std::cout << "\e[0;33mserver listen on port \e[0m" << this->_connection_port << std::endl;
    // FD_ZERO (&fds);
    // FD_SET(this->_server_fd, &fds);
    // while (this->_server_alive)
    // {
    //     ready_fds = fds;
    //     err_check = select(FD_SETSIZE, &ready_fds, NULL, NULL, NULL) ;
    //     if ( err_check == -1)
    //         throw Connection_error ("select error");
    //     for (int i = this->_server_fd; i < (this->_server_fd + this->_request_count + 1); i++)
    //     {
    //         this->handle_active_sockets (i);
    //     }
    // } 
    return (0); 
}

void Server_instance::bind_socket (void)
{
    this->_server_fd = socket (AF_INET, SOCK_STREAM, 0);
    fcntl (this->_server_fd, F_SETFL, O_NONBLOCK);
    bzero (this->_buffer, HEADER_MAX);
    if (this->_server_fd == -1)
        throw  Fatal_error();
    else
    {
        this->err_check =  setsockopt (this->_server_fd, SOL_SOCKET, SO_REUSEADDR, &this->enable, sizeof (int));
        if (this->err_check == -1)
            throw  Fatal_error();
        this->_server_addr.sin_family = AF_INET; 
        this->_server_addr.sin_addr.s_addr = INADDR_ANY;
        if (this->_connection_port > 0)
            this->_server_addr.sin_port = htons( this->_connection_port);
        this->err_check = bind (this->_server_fd, (struct sockaddr*) &this->_server_addr, sizeof (this->_server_addr)) ;
        if (this->err_check == -1)
            throw  Fatal_error();
    }
} 


int Server_instance::getSocketFd (void) const
{
    return (this->_server_fd);
}

int Server_instance::getRequestCount (void) const
{
    return (this->_request_count);    
}

const char *Fatal_error::what () const throw ()
{
    return "\e[0;31m FATAL ERROR \e[0m";
}

Connection_error::Connection_error (const char *desc): err_description (desc)
{
}

const char * Connection_error::what () const throw (){
    return (this->err_description);
}



