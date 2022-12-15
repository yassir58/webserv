#include "basic_cn_server.hpp"


Server_instance::Server_instance (void)
{
    this->_server_fd = socket (AF_INET, SOCK_STREAM, 0);
    this->_server_alive = 1;
    bzero (this->_buffer, HEADER_MAX);
    this->enable = 1;
    if (this->_server_fd == -1)
        throw  Fatal_error();
    else
    {
        this->err_check =  setsockopt (this->_server_fd, SOL_SOCKET, SO_REUSEADDR, &this->enable, sizeof (int)  );
        if (this->err_check == -1)
            throw  Fatal_error();
        this->_server_addr.sin_family = AF_INET; 
        this->_server_addr.sin_addr.s_addr = INADDR_ANY; 
        // this->_server_addr.sin_port = htons( PORT ); 
        this->err_check = bind (this->_server_fd, (struct sockaddr*) &this->_server_addr, sizeof (this->_server_addr)) ;
        if (this->err_check == -1)
            throw  Fatal_error();
    }
}


Server_instance::~Server_instance (void)
{
    std::cout << "\e[0;31m shutdown server \e[0m" << std::endl;
}

// Server_instance::Server_instance (const Server_instance &copy)
// {
//     this->_server_fd = socket (AF_INET, SOCK_STREAM, 0);
//     this->_server_alive = 1;
//     bzero (this->_buffer, HEADER_MAX);
///    this->enable = 1;
//     if (this->_server_fd == -1)
//         throw  Fatal_error();
//     else
//     {
//         this->err_check =  setsockopt (this->_server_fd, SOL_SOCKET, SO_REUSEADDR, &this->enable, sizeof (int)  );
//         if (this->err_check == -1)
//             throw  Fatal_error();
//         this->_server_addr.sin_family = AF_INET; 
//         this->_server_addr.sin_addr.s_addr = INADDR_ANY; 
//         // this->_server_addr.sin_port = htons( PORT ); 
//         this->err_check = bind (this->_server_fd, (struct sockaddr*) &this->_server_addr, sizeof (this->_server_addr)) ;
//         if (this->err_check == -1)
//             throw  Fatal_error();
//     }
// }

//Server_instance &Server_instance::operator= (const Server_instance &assign);

Server_instance::Server_instance (int port, std::string name)
{
    this->_server_fd = socket (AF_INET, SOCK_STREAM, 0);
    this->_connection_port = port;
    this->_server_alive = 1;
    this->_server_name = name;
    bzero (this->_buffer, HEADER_MAX);
    this->enable = 1;
    if (this->_server_fd == -1)
        throw  Fatal_error();
    else
    {
        this->err_check =  setsockopt (this->_server_fd, SOL_SOCKET, SO_REUSEADDR, &this->enable, sizeof (int)  );
        if (this->err_check == -1)
            throw  Fatal_error();
        this->_server_addr.sin_family = AF_INET; 
        this->_server_addr.sin_addr.s_addr = INADDR_ANY; 
        this->_server_addr.sin_port = htons( this->_connection_port); 
        this->err_check = bind (this->_server_fd, (struct sockaddr*) &this->_server_addr, sizeof (this->_server_addr)) ;
        if (this->err_check == -1)
            throw  Fatal_error();
    }
}

int Server_instance::establish_connection (void)
{
    FD_ZERO (&this->_current_fds);
    FD_SET (this->_server_fd, &this->_current_fds);
    this->err_check = listen (this->_server_fd, BACK_LOG_MAX) ;
    if (this->err_check == -1)
        throw  Connection_error("LISTEN ERROR");
    std::cout << "\e[0;32m server listen on port \e[0m" << this->_connection_port << std::endl;
    while (this->_server_alive)
    {
        this->_active_fds = this->_current_fds;
        if (select (FD_SETSIZE, &this->_active_fds, NULL, NULL, NULL) == -1)
            throw  Connection_error("SELECT ERROR");
        // !optimization
        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET (i, &this->_active_fds))
                this->handle_active_sockets (i);
        }
    }
    return (0); 
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
};
