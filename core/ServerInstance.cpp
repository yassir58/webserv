#include "ServerInstance.hpp"
#include "../response/Response.hpp"

ServerInstance::ServerInstance (void)
{
    this->enable = 1;
    this->requestCount = 0;
    status = 1;
    this->connectionPort = 8080;
    this->hostName = "127.0.0.1";
}

ServerInstance::~ServerInstance (void)
{
    std::cout << "\e[0;31m shutdown server \e[0m" << std::endl;
    close (this->serverSocket);
}

ServerInstance::ServerInstance (const ServerInstance &copy)
{
    this->enable = copy.enable;
    this->requestCount = copy.requestCount;
    status = 1;
    this->connectionPort = copy.connectionPort;
    this->hostName = copy.hostName;
}

ServerInstance &ServerInstance::operator= (const ServerInstance &assign)
{
    this->enable = assign.enable;
    this->requestCount = assign.requestCount;
    status = 1;
    this->connectionPort = assign.connectionPort;
    this->hostName = assign.hostName;
    return (*this);
}

ServerInstance::ServerInstance (std::string host, int port)
{   status = 1;
    this->hostName = host;
    this->connectionPort = port;
    this->setService (port);
    this->requestCount = 0;
    this->enable = 1;
}

std::string ServerInstance::getService (void) const
{
    return (this->service);
}

int ServerInstance::establish_connection (void)
{
    this->bind_socket ();
    errCheck = listen (this->serverSocket, BACK_LOG_MAX);     
    if (errCheck == -1)
        throw Connection_error (strerror (errno), "listen");
    else
        std::cout << "\e[0;33m listening on port \e[0;32m" << this->connectionPort << "\e[0;36m [ " << this->hostName  << " ]\e[0m" <<   std::endl;
    return (0); 
}

void ServerInstance::bind_socket (void)
{
    struct addrinfo initAddr;
    struct addrinfo  *servAddr;

    memset (&initAddr, 0 , sizeof (initAddr));
    initAddr.ai_family = AF_INET; // this need to be modified
    initAddr.ai_socktype = SOCK_STREAM;
    initAddr.ai_flags = AI_PASSIVE; // this flag make the getaddreinfo info return wildcard address
    errCheck = getaddrinfo (hostName.c_str (), service.c_str (), &initAddr, &servAddr);
    if (errCheck == -1)
        throw Connection_error (gai_strerror (errno), "getaddrinfo");
    this->serverSocket = socket (servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
    if (this->serverSocket < 0)
    {
        freeaddrinfo(servAddr);
        throw Connection_error (strerror (errno), "socket");
    }
    this->errCheck =  setsockopt (this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &this->enable, sizeof (int));
    if (this->errCheck == -1)
        throw  Connection_error (strerror (errno), "setsocketopt");
    fcntl (this->serverSocket, F_SETFL, O_NONBLOCK);
    // int option = 0;
    // errCheck = setsockopt (this->serverSocket, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&option, sizeof (option));
    // if (errCheck == -1)
    //     throw  Connection_error (strerror (errno), "setsocketopt");
    this->errCheck = bind (this->serverSocket, servAddr->ai_addr, servAddr->ai_addrlen) ;
    if (this->errCheck == -1)
        throw  Connection_error (strerror (errno), "bind");
    freeaddrinfo(servAddr);
} 


int ServerInstance::getSocketFd (void) const
{
    return (this->serverSocket);
}

std::string ServerInstance::getHostName (void)
{
    return (this->hostName);
}

int ServerInstance::getRequestCount (void) const
{
    return (this->requestCount);    
}


Fatal_error::Fatal_error (const char *desc):err_description (desc)
{}

const char *Fatal_error::what () const throw ()
{
    return err_description;
}

Connection_error::Connection_error (const char* desc, const char* context): err_description (desc), context (context)
{
}

Parse_error::Parse_error (const char *desc): err_description (desc)
{}

const char * Connection_error::what () const throw (){

    std::cerr << context << " : ";
    return (err_description);
}

const char *Parse_error::what () const throw()
{
    return (this->err_description);
}


void ServerInstance::setService (int port)
{
    std::stringstream strs;

    strs << port ;
    strs >> service;
}

void ServerInstance::setService (std::string service)
{
    this->service = service;
}


void ServerInstance::setStatus (int status)
{
    this->status = status;    
}

int ServerInstance::getStatus (void) const
{
    return (this->status);
}