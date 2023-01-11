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
#include <fcntl.h>
#include <fstream>
#include <vector>
#include <list>
#include <string> 
#include <algorithm>
#include <sys/types.h>
#include <netdb.h>
#include "../config/config.hpp"
#include "../request/Request.hpp"
#include <sstream>
#include <cstring>
 #include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

// MACROS
#define PORT 8080
#define BUFFER_MAX 8000
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
#define TRUE 1
#define FALSE 0


typedef std::vector <Server*> serverBlocks;
typedef int SOCKET ;


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
        const char *context;
    public:
        virtual const char *what() const throw();
        Connection_error(const char *desc, const char *context);
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
        std::string hostName;
        int requestCount;
        SOCKET serverSocket;
        unsigned int connectionPort;
        std::string service;
        int enable;
        int errCheck;
        struct addrinfo addr;
        int status;

public:
    ServerInstance(void);                       // init server with random port number
    ServerInstance(std::string host, int port); // init server with the given port
    ~ServerInstance();
    ServerInstance(const ServerInstance &copy);
    ServerInstance &operator=(const ServerInstance &assign);
    void bind_socket(void);
    int establish_connection(void);
    int accept_connection(void);
    int getSocketFd(void) const;
    int getRequestCount(void) const;
    void setServerPort(unsigned int port);
    unsigned int getServerPort (void);
    std::string getHostName (void);
    void setService (int port);
    void setService (std::string service);
    std::string getService (void) const;
    void setStatus (int status);
    int getStatus (void) const;
};

typedef std::vector <ServerInstance*> serverContainer ;

class HttpApplication
{
    private:
        int epollInstance;
		int queueIdentifier;
        struct kevent events[MAX_CONNECT];
		int eventIndx;
        int nfds;
        int errValue;
        int serverCount;
        int connectionCount;
        int returnValue;
        int indx;
        serverContainer serverList;
        serverBlocks servConfigBlocks;
        std::string httpDefaultErrorPage;
        int HttpMaxBodySize;
        std::ofstream logFile;
        std::vector<int> serverFds;
        Config *config;
      

public:
    HttpApplication();
    HttpApplication(const HttpApplication &copy);
    ~HttpApplication();
    int getServerCount(void) const;
    int getConnectionCount(void) const;
    void connectServers(void);
    void setupAppResources (void);
    void handleNewConnection(int serverFd);
    void handleConfig (int argc, char *argv[]);
    pollfd *getConnectionPool(void) const;
    serverContainer getServerList(void) const;
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
    void filterServerBlocks (void);
    int checkServerExistance (Server *block);
    ServerInstance *findServerByFd (int serverFd);
	int isServer (int fd);

};


class Client {
    private:
        Request *request;
        int clientSocket;
        std::vector <int> resolversList;
        int serverHandlerIndx;
        char buffer[BUFFER_MAX];
        int dataRecievedLength;
        struct addrinfo *requestSourceAddr;
        int clientPort;
        std::string clientIp;

    public:
        Client ();
        Client (int fd);
        int getClientSocket (void) const;
        int getServerHandlerIndx (void) const;
        char *getBuffer (void) ;
        void emptyBuffer (void);
        void recieveData (void);
        void sendData (void);
        std::vector <int> getResolversList (void) const;
        void generateResolversList (serverContainer serverList);
};


void handleError(int err);
void initServers(ServerInstance *serv_list);
const std::string currentDateTime();


#endif