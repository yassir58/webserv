#pragma once
#ifndef SERVER_INSTANCE_HPP
#define SERVER_INSTANCE_HPP

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <sys/select.h>
#include <poll.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <fstream>
#include <vector>
#include <list>
#include <string> 
#include <algorithm>
#include <sys/types.h>
#include <netdb.h>
#include "../config/config.hpp"
#include <sstream>
#include <cstring>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()
// MACROS
#define PORT 8080
#define HEADER_MAX 8000
#define MAX_CONNECT 1024
#define POLL_TIMEOUT 5000
#define BUFFER_SIZE 10
#define POST_LIMIT 100000
#define GET8LIMIT 100000
#define READERR 1
#define ACCEPTERR 2
#define LISTENERR 3
#define BINDERR 4
#define POLLERROR 5
#define WRITEERR 6
#define CLOSEERR 7
#define POLL_INF -1
#define BACK_LOG_MAX 5 // * max size permitted by most systems
#define ACK_MESSAGE "\e[0;33m acknowledgement message \e[0m"
#define RESPONSE_LINES 7
#define DEFAULT_PORT 8080
#define DEFAULT_SERVER_NAME "default"

// for testing purpos
#define HTTP_RESPONSE_EXAMPLE "HTTP/1.1 200 OK\r\nServer: WebServer\r\nContent-Type: text/html\r\nContent-Length: 109\r\nConnection: close\r\n\r\nhello world</br><p>this is a paragraph</p><img src='https://i.ytimg.com/vi/8wWBcs99hTw/hqdefault.jpg' ></img>"
class Fatal_error : public std::exception
{
    const char *err_description ;
    public:
        virtual const char *what() const throw();
        Fatal_error (const char* desc);
};

class Parse_error : public std::exception
{
    const char *err_description ;
    public:
        virtual const char *what() const throw();
        Parse_error (const char *desc);
};

class Connection_error : public std::exception
{
        const char *err_description;
    public:
        virtual const char *what() const throw();
        Connection_error(const char *desc);
};

class Connection
{
    private:
        int fd;
        fd_set timeOutSet;
        int fd_type;
    public:
        void setFd (int fd);
        void initFdSet (int fd);
        void setFdType (int type);
};

class ServerInstance
{
    private:
        std::string _server_name;
        int _request_count;
        int _server_fd;
        int _connection_fd;
        int _connection_port;
        std::string _service;
        int _addr_len;
        fd_set fds;
        fd_set ready_fds;
        std::string _request_text;
        int _server_alive;
        int _read_return;
        char _buffer[HEADER_MAX];
        int err_check;
        int enable;
public:
    ServerInstance(void);                       // init server with random port number
    ServerInstance(int port, std::string name); // init server with the given port
    ~ServerInstance();
    ServerInstance(const ServerInstance &copy);
    ServerInstance &operator=(const ServerInstance &assign);
    void bind_socket(void);
    int establish_connection(void);
    int accept_connection(void);
    void handle_request(int conn_fd);
    void handle_active_sockets(int i);
    int getSocketFd(void) const;
    int getRequestCount(void) const;
    std::string getServerName (void) const;
    void setServerName(std::string name);
    void setServerPort(int port);
    int getServerPort (void);
    void setService (int port);
    void setService (std::string service);

  
};

class Http_application
{
    private:
        int epollInstance;
        struct epoll_event readySockets[MAX_CONNECT];
        int nfds;
        int errValue;
        int serverCount;
        int connectionCount;
        int return_value;
        int indx;
        char buffer[HEADER_MAX];
        std::vector <ServerInstance*> serverList;
        std::string defaultErrorPage;
        int clientMaxBodySize;
        std::ofstream logFile;
        std::list<int> watchedFds;
        std::vector<int> serverFds;
        std::vector <Server*> servConfigs;
        Config *config;
      


public:
    Http_application();
    Http_application(const Http_application &copy);
    ~Http_application();
    int getServerCount(void) const;
    int getConnectionCount(void) const;
    void connectServers(void);
    void setupAppResources (void);
    void handleNewConnection(int server_indx);
    void handleConfig (int argc, char *argv[]);
    pollfd *getConnectionPool(void) const;
    ServerInstance *getServerList(void) const;
    // void setConnectionPool(pollfd *fd_pool);
    void allocateServers (void);
    void checkForConnection (void);
    void handleHttpRequest (int fd);
    int getConnectionIndx (void);
    int getConnectionCount (void);
    // void setConnectionFd (int fd, int type);
    // void unsetConnectionFd (int fd);
    // int chechTimeOut (int fd);
    // int getFdType (int fd);
    void serverLog (int serverIndx);
    void printServerInfo (void);


};


class Client {
    private:
        int clientSocket;
    public:
};

class request
{
};

void handleError(int err);
void initServers(ServerInstance *serv_list);
const std::string currentDateTime();


#endif