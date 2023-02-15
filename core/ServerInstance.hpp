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
#include <sstream>
#include <cstring>
 #include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <signal.h>
#include <dirent.h>
#include <sys/ioctl.h>


// * - * --------------------- MACROS --------------------- * - * //

#define PORT 8080
#define BUFFER_MAX 65000
#define MAX_CONNECT 1024
#define POLL_TIMEOUT 5000
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
#define HTTP_RESPONSE_EXAMPLE "HTTP/1.1 200 OK\r\nServer: WebServer\r\nContent-Type: text/html\r\nContent-Length: 23\r\nConnection: close\r\n\r\n<h1> hello world </h1>"
#define HTTP_LENGTH 225

// * - * --------------------- TYPE DEFINITIONS --------------------- * - * //


typedef std::vector <Server*> serverBlocks;
typedef int SOCKET ;
typedef std::vector <int> intContainer;
typedef std::vector <std::string> stringContainer;

// * - * --------------------- FORWARD DECLARATIONS --------------------- * - * //

class Request;
class Response;
class CGIHandler ;

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

        int enable;
        int errCheck;
        int status;
        int requestCount;
        unsigned int connectionPort;
        std::string hostName;
        std::string service;
        SOCKET serverSocket;
        struct addrinfo addr;

	public:

		// * - * --------------------- CONSTRUCTORS --------------------- * - * //

    	ServerInstance(void);                       
    	ServerInstance(std::string host, int port);
    	~ServerInstance();
    	ServerInstance(const ServerInstance &copy);
    	ServerInstance &operator=(const ServerInstance &assign);

		// * - * --------------------- GETTER FUNCTIONS --------------------- * - * //

    	int				getSocketFd(void) const;
    	int				getStatus (void) const;
    	int				getRequestCount(void) const;
    	unsigned int	getServerPort (void);
    	std::string		getHostName (void);
    	std::string		getService (void) const;

		// * - * --------------------- SETTER FUNCTIONS --------------------- * - * //

    	void			setService (int port);
    	void			setService (std::string service);
    	void			setStatus (int status);
    	void			setServerPort(unsigned int port);
    	void			bind_socket(void);
    	int				establish_connection(void);
    	int				accept_connection(void);
};

typedef std::vector <ServerInstance*> serverContainer ;

class Connection {
    
	private:
        int				ConnectionSocket;
		int 			status;
        int 			dataReminder;
        int 			dataToRead ;
        int 			ContentLength;
        int 			headerLength;
        int 			upload;
        int 			bodyRead;
        int 			readStatus;
        int 			dataRecievedLength;
        int 			ConnectionPort;
		int 			responseIndex;
		size_t 			bytesSent ;
		size_t 			requestLength;
        std::vector 	<int>resolversList;
        std::vector 	<char>requestBody;
		CGIHandler 		*CGI;
		serverBlocks 	servList;
        Request 		*request;
		Response 		*response;
        std::string 	requestHeader;
		std::string 	serviceName;
        std::string 	hostName;
		std::string		ipAddress;
        struct addrinfo	*requestSourceAddr;
        char 			httpBuffer[BUFFER_MAX + 1];
		Config 			*conf;
		bool 			responseConstructed;
		bool 			cgi;

    public:

		// * - * --------------------- CONSTRUCTORS --------------------- * - * //

        Connection ();
		Connection (int fd);
		~Connection ();

		// * - * --------------------- GETTER FUNCTIONS --------------------- * - * //

        int					getConnectionSocket (void) const;
        char 				*getBuffer (void) ;
		size_t 				getBytesSent (void) const ;
		bool 				getResponseState (void) const;
        int					getRequestLength (void) const ;
        int					getContentLength (void) const;
        int					getUpload (void) const;
        int					getDataToRead (void) const;
        int					getBodyRead (void) const;
		Request				*getRequest (void) const;
        std::vector<char> 	getRequestBody (void) const;
        std::string 		getRequestHeaders (void) const;
		serverBlocks 		getServerBlocks (void) const;
		Config 				*getConfig (void) const;
        std::vector <int> 	getResolversList (void) const;

		// * - * --------------------- SETTER FUNCTIONS --------------------- * - * //

		void 				setRequest (void);
		void 				setStatus (int status);
		void 				setServerBlocks (serverBlocks serverList);
		void 				setConfig (Config *conf);

		// * - * --------------------- HELPER FUNCTIONS --------------------- * - * //

        void				emptyBuffer (void);
        int 				recieveData (int *start, int *len);
        void 				generateResolversList (serverBlocks serverList);
		void 				printfResolvers (void);
        void 				appendBuffer (size_t start, int dataRecived);
		int 				sendResponse (int fd);
		void	 			constructResponse (void);
		void				extractHeaderLength (std::string tmpBuffer, int *start, int *len);
		void				extractContentLength (std::string tmpBuffer);
		void				extractMethod (std::string tmpBuffer);
};

typedef std::vector <Connection *> connectionPool;

class HttpApplication
{
    private:

		// * - * --------------------- HELPERS --------------------- * - * //

        int nfds;
        int errValue;
        int serverCount;
        int connectionCount;
        int returnValue;
        int indx;
		int fdMax;

		// * - * --------------------- SERVER LOG FILES --------------------- * - * //

        std::ofstream accessLog;
		std::ofstream errorLog ;
        std::ofstream binFile;
		std::ofstream file;
        Config *config;
        serverContainer serverList;
		connectionPool connections;
		// * - * --------------------- SELECT VARIABLES --------------------- * - * //
		fd_set readFds, writeFds, errorFds;
        intContainer serverFds;
		intContainer watchedFds;
        intContainer openConnections;

	public:

		// * - * --------------------- CONSTRUCTORS --------------------- * - * //

    	HttpApplication();
    	HttpApplication(const HttpApplication &copy);
    	~HttpApplication();

		// * - * --------------------- GETTER FUNCTIONS --------------------- * - * //

    	int							getServerCount(void) const;
    	int							getConnectionCount(void) const;
    	int							getConnectionIndx (void);
    	int							getConnectionCount (void);
    	pollfd						*getConnectionPool(void) const;
    	serverContainer				getServerList(void) const;
		serverBlocks				getServerBlockList (void) const;
		Config 						*getConfig (void) const;

		// * - * -------			-------------- SERVER FUNCTIONS --------------------- * - * //

    	void 						connectServers(void);
    	void 						handleNewConnection(int serverFd);
    	void 						handleConfig (int argc, char *argv[]);
    	void 						allocateServers (void);
    	void 						checkForConnection (void);
    	void 						handleHttpRequest (int fd);
		void 						handleHttpResponse (int fd);
    	void 						serverLog (int serverIndx);
    	void 						printServerInfo (void);
    	void 						filterServerBlocks (void);
    	int							checkServerExistance (Server *block);
    	ServerInstance				*findServerByFd (int serverFd);
		int 						isServer (int fd);
		void 						initServerSet (void);
		connectionPool::iterator	getConnection (int fd);
		void						connectionAccessLog (std::string msg, int requestLength, std::string addr, std::string port);
		void						connectionErrorLog (std::string errorContext, std::string errorMessage, std::string addr, std::string port);
		void						handleSigPipe ();
		void						closeConnection (SOCKET fd, std::string error);
		void						removeConnection (SOCKET fd);
		void						closeConnection (SOCKET fd);
};

// * - * --------------------- HELPER FUNCTIONS --------------------- * - * //

void handleError(int err);
void initServers(ServerInstance *serv_list);
const std::string currentDateTime();
std::string getTestBody (std::string filename);
std::string listDirectory (std::string dirPath);
#endif

