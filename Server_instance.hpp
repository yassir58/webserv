#ifndef SERVER_INSTANCE_HPP
#define SERVER_INSTANCE_HPP

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <iostream>
#include <sys/select.h>
#include <poll.h>
#include <fcntl.h>
#include <fstream>

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
#define POLL_INF -1
#define BACK_LOG_MAX 5 // * max size permitted by most systems
#define ACK_MESSAGE "\e[0;33m acknowledgement message \e[0m"
#define RESPONSE_LINES 7
// for testing purpos
#define HTTP_RESPONSE_EXAMPLE "HTTP/1.1 200 OK\r\nServer: WebServer\r\nContent-Type: text/html\r\nContent-Length: 42\r\nConnection: close\r\n\r\nhello world</br><p>this is a paragraph</p>"
class Fatal_error : public std::exception
{
    const char *what() const throw();
};

class Connection_error : public std::exception
{
    const char *what() const throw();
    const char *err_description;

public:
    Connection_error(const char *desc);
};

class Server_instance
{
private:
    std::string _server_name;
    int _request_count;
    int _server_fd;
    int _connection_fd;
    int _connection_port;
    int _addr_len;
    sockaddr_in _server_addr;
    fd_set fds;
    fd_set ready_fds;
    std::string _request_text;
    int _server_alive;
    int _read_return;
    char _buffer[HEADER_MAX];
    int err_check;
    int enable;
    Fatal_error f_err;

public:
    Server_instance(void);                       // init server with random port number
    Server_instance(int port, std::string name); // init server with the given port
    ~Server_instance();
    Server_instance(const Server_instance &copy);
    Server_instance &operator=(const Server_instance &assign);
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
  
};

class Http_application
{
    private:
        pollfd *fd_pool;
        int err_value;
        int server_count;
        int connection_count;
        int server_range_start;
        int server_range_end;
        int return_value;
        int indx;
        char buffer[HEADER_MAX];
        Server_instance *server_list;
        std::string default_error_page;
        int client_max_body_size;
        std::ofstream log_file;

public:
    Http_application();
    Http_application(int servers_n);
    Http_application(const Http_application &copy);
    ~Http_application();
    void setServerList(Server_instance *list);
    int getServerCount(void) const;
    int getServerRangeStart(void) const;
    int getServerRangeEnd(void) const;
    int getConnectionCount(void) const;
    void connectServers(void);
    void initServers(void);
    void handleNewConnection(int server_indx);
    void handleReadyConnection(int indx);
    pollfd *getConnectionPool(void) const;
    Server_instance *getServerList(void) const;
    void setConnectionPool(pollfd *fd_pool);
    void allocate_servers (void);
    void checkReadySockets (void);
    void handleHttpRequest (int client_indx);
    int getConnectionIndx (void);

};

class connection_state
{
};

class request
{
};

void handle_error(int err);
void initServers(Server_instance *serv_list);
const std::string currentDateTime();

#endif