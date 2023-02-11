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
#include <sys/time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <signal.h>
#include <dirent.h>
#include <sys/ioctl.h>

// MACROS
#define PORT 8080
#define BUFFER_MAX 65000
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
#define BACK_LOG_MAX 50 // * max size permitted by most systems
#define ACK_MESSAGE "\e[0;33m acknowledgement message \e[0m"
#define RESPONSE_LINES 7
#define DEFAULT_PORT 8080
#define DEFAULT_SERVER_NAME "default"
#define TRUE 1
#define FALSE 0
#define OPEN 1
#define CLOSE 0
#define CRLF "\r\n\r\n"


typedef std::vector <Server*> serverBlocks;
typedef int SOCKET ;
typedef std::vector <int> intContainer;
typedef std::vector <std::string> stringContainer;


// for testing purpos
#define HTTP_RESPONSE_EXAMPLE "HTTP/1.1 200 OK\r\nServer: WebServer\r\nContent-Type: text/html\r\nContent-Length: 23\r\nConnection: close\r\n\r\n<h1> hello world </h1>"
#define HTTP_LENGTH 225
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

class Connection {
    private:
        Request *request;
        int ConnectionSocket;
        std::vector <int> resolversList;
        char *httpBuffer;
        std::vector <char> requestData;
        int dataRecievedLength;
        struct addrinfo *requestSourceAddr;
        int ConnectionPort;
        std::string hostName;
		std::string serviceName;
		std::string ipAddress;
		size_t requestLength;
		int status;
        std::string requestString;
        int dataReminder;
        int dataToRead ;
        int ContentLength;
        int headerLength;
        int upload;
        int bodyRead;
        int readStatus;
        std::string requestHeader;
    public:
        Connection ();
		Connection (int fd);
		~Connection ();
        int getConnectionSocket (void) const;
        char *getBuffer (void) ;
        void emptyBuffer (void);
        int recieveData (int *start, int *len);
        void sendResponse (void);
        std::vector <int> getResolversList (void) const;
        void generateResolversList (serverBlocks serverList);
		void setRequest (serverBlocks serverList);
		Request *getRequest (void) const;
		void printfResolvers (void);
		void setStatus (int status);
        void appendBuffer (size_t start, int dataRecived);
        int getRequestLength (void) const ;
        std::string getRequestString (void) const;
        int getDataToRead (void) const;
        void setDataTorRead (int dataTorRead) ;
        int getContentLength (void) const;
        int getUpload (void) const;
        int getBodyRead (void) const;
        std::vector<char> getRequestData (void) const;
        std::string getRequestHeader (void) const;
};

typedef std::vector <Connection *> connectionPool;

class HttpApplication
{
    private:
		int queueIdentifier;
        int nfds;
        int errValue;
        int serverCount;
        int connectionCount;
        int returnValue;
        int indx;
        serverContainer serverList;
        std::string httpDefaultErrorPage;
        int HttpMaxBodySize;
        std::ofstream accessLog;
		std::ofstream errorLog ;
        std::ofstream binFile;
        intContainer serverFds;
		intContainer watchedFds;
        Config *config;
		connectionPool connections;
		fd_set readFds, writeFds, errorFds;
		 std::ofstream file;
		int fdMax;
        intContainer openConnections;
    
public:
    HttpApplication();
    HttpApplication(const HttpApplication &copy);
    ~HttpApplication();
    int getServerCount(void) const;
    int getConnectionCount(void) const;
    void connectServers(void);
    void handleNewConnection(int serverFd);
    void handleConfig (int argc, char *argv[]);
    pollfd *getConnectionPool(void) const;
    serverContainer getServerList(void) const;
	serverBlocks getServerBlockList (void) const;
    // void setConnectionPool(pollfd *fd_pool);
    void allocateServers (void);
    void checkForConnection (void);
    void handleHttpRequest (int fd);
	void handleHttpResponse (int fd);
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
	void initServerSet (void);
	connectionPool::iterator getConnection (int fd);
	/// testing log
	void connectionAccessLog (std::string msg, int requestLength, std::string addr, std::string port);
	void connectionErrorLog (std::string errorContext, std::string errorMessage, std::string addr, std::string port);
	// testing 
	void printServerFds (void)
	{
		std::vector <int>::iterator it;

		for (it = serverFds.begin () ; it != serverFds.end (); it++)
			std ::cout << "\e[0;36m " << (*it) << " \e[0m";
		std::cout << std::endl;
	}
	void handleSigPipe ();
	Config *getConfig (void) const;

};


void handleError(int err);
void initServers(ServerInstance *serv_list);
const std::string currentDateTime();
std::string getTestBody (std::string filename);
std::string listDirectory (std::string dirPath);
#endif

