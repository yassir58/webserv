#include "ServerInstance.hpp"


ServerInstance::ServerInstance (void)
{
    this->_server_alive = 1;
    this->enable = 1;
    this->_request_count = 0;
    this->_connection_port = 0;
}

ServerInstance::~ServerInstance (void)
{
    std::cout << "\e[0;31m shutdown server \e[0m" << std::endl;
    close (this->_server_fd);
}

ServerInstance::ServerInstance (const ServerInstance &copy)
{
    this->_server_name = copy._server_name;
    this->_connection_port = copy._connection_port;
}

ServerInstance &ServerInstance::operator= (const ServerInstance &assign)
{
    this->_server_name = assign._server_name;
    this->_connection_port = assign._connection_port;
    return (*this);
}

ServerInstance::ServerInstance (int port, std::string name)
{
    this->_connection_port = port;
    this->_server_alive = 1;
    this->_request_count = 0;
    this->_server_name = name;
    this->enable = 1;
}

int ServerInstance::establish_connection (void)
{
    this->bind_socket ();
    err_check = listen (this->_server_fd, BACK_LOG_MAX);     
    if (err_check == -1)
    {
        handleError (LISTENERR);
        throw Connection_error (strerror (errno));
    }
    else
        std::cout << this->_server_name << "\e[0;33m listen on port \e[0m" << this->_connection_port << std::endl;
    return (0); 
}

void ServerInstance::bind_socket (void)
{
    struct addrinfo initAddr;
    struct addrinfo  *servAddr;

    memset (&initAddr, 0 , sizeof (initAddr));
    initAddr.ai_family = AF_INET6; // this need to be modified
    initAddr.ai_socktype = SOCK_STREAM;
    initAddr.ai_flags = AI_PASSIVE; // this flag make the getaddreinfo info return wildcard address
    err_check = getaddrinfo (0, _service.c_str (), &initAddr, &servAddr);
    if (err_check == -1)
    {
        std::cout << "context : getaddrinfo" << std::endl;
        throw Connection_error (strerror (errno));
    }
    this->_server_fd = socket (servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
    if (this->_server_fd < 0)
    {
         std::cout << "context : socket" << std::endl;
        throw Connection_error (strerror (errno));
    }
    this->err_check =  setsockopt (this->_server_fd, SOL_SOCKET, SO_REUSEADDR, &this->enable, sizeof (int));
    if (this->err_check == -1)
    {
        std::cout << "context : setsocketopt" << std::endl;
        throw  Connection_error (strerror (errno));
    }
    fcntl (this->_server_fd, F_SETFL, O_NONBLOCK);
    bzero (this->_buffer, HEADER_MAX);
    int option = 0;
    err_check = setsockopt (this->_server_fd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&option, sizeof (option));
    if (err_check == -1)
    {
        std::cout << "context : setsocketopt" << std::endl;
        throw  Connection_error (strerror (errno));
    }
    this->err_check = bind (this->_server_fd, servAddr->ai_addr, servAddr->ai_addrlen) ;
    if (this->err_check == -1)
    {
        handleError (BINDERR);
        throw  Connection_error (strerror (errno));
    }
    freeaddrinfo(servAddr);
} 


int ServerInstance::getSocketFd (void) const
{
    return (this->_server_fd);
}

int ServerInstance::getRequestCount (void) const
{
    return (this->_request_count);    
}


Fatal_error::Fatal_error (const char *desc):err_description (desc)
{}

const char *Fatal_error::what () const throw ()
{
    return err_description;
}

Connection_error::Connection_error (const char *desc): err_description (desc)
{
}

Parse_error::Parse_error (const char *desc): err_description (desc)
{}

const char * Connection_error::what () const throw (){
    return (this->err_description);
}

const char *Parse_error::what () const throw()
{
    return (this->err_description);
}


void ServerInstance::setService (int port)
{
    std::stringstream strs;

    strs << port ;
    strs >> _service;
}

void ServerInstance::setService (std::string service)
{
    _service = service;
}
