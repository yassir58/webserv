#include "Server_instance.hpp"


Http_application::Http_application ()
{
    server_count = 1;
    connection_count = 0;
    server_range_start = 0;
    server_range_end = 0;
    client_max_body_size = POST_LIMIT;
    bzero (buffer, HEADER_MAX);
    log_file.open ("server_log", std::ios_base::app);
    this->fd_pool = new pollfd[MAX_CONNECT];
    indx = server_count;

}

Http_application::Http_application (int servers_n)
{
    server_count = servers_n;
    connection_count= 0;
    server_range_start = 0;
    server_range_end = servers_n;
    client_max_body_size = POST_LIMIT;
    bzero (buffer, HEADER_MAX);
    log_file.open ("server_log", std::ios_base::app);
    this->fd_pool = new pollfd[MAX_CONNECT];
    indx = server_count;
}

Http_application::~Http_application ()
{
    std::cout << "\e[0;31m HTTP APPLICATION CLOSED \e[0m" <<std::endl;
    delete [] server_list;
    delete [] fd_pool;
    log_file.close ();
}

Http_application::Http_application (const Http_application &copy)
{
    server_count = copy.server_count;
    connection_count= copy.connection_count;
    server_range_start = copy.server_range_start;
    server_range_end = copy.server_range_end;
    client_max_body_size = copy.client_max_body_size;
    indx = server_count;
}


int Http_application::getServerCount (void) const
{
    return (this->server_count);
}

int Http_application::getServerRangeStart (void) const
{
    return (this->server_range_start);
}
int Http_application::getServerRangeEnd (void) const
{
    return (this->server_range_end);
}

int Http_application::getConnectionCount (void) const
{
    return (this->connection_count);
}

void Http_application::connectServers (void)
{
    for (int i = 0; i <  3 ; i++)
    {
        this->server_list[i].establish_connection ();
        this->fd_pool[i].fd = server_list[i].getSocketFd ();
        this->fd_pool[i].events = POLLIN | POLLOUT;
    }
}

Server_instance *Http_application::getServerList (void) const
{
    return (this->server_list);
}

void Http_application::checkReadySockets (void)
{
    this->err_value = poll (fd_pool, MAX_CONNECT, POLL_INF);
    if (this->err_value < 0)
        handle_error (POLLERROR);
}

void Http_application::handleNewConnection (int server_indx)
{
    fd_pool[indx].fd = server_list[server_indx].accept_connection();
    fd_pool[indx].events = POLLIN;
    indx++;
    log_file << "connection count : "<< connection_count << " ";
    log_file <<  "\e[0;32mserver name: \e[0m: " <<     server_list[server_indx].getServerName () << " ";
    log_file << "\e[0;32mserver port: \e[0m:" << server_list[server_indx].getServerPort () << " ";
    log_file << " " << currentDateTime ()  <<  std::endl;
    connection_count++;
}

void Http_application::handleReadyConnection (int indx)
{
    if (fd_pool[indx].revents == POLLIN || fd_pool[indx].events == POLLOUT)
    {
        if (indx >=this->getServerRangeStart () && indx <this->getServerRangeEnd ())
            handleNewConnection (indx);
        else
        {
            this->handleHttpRequest (indx);   
        }
    }
}

void Http_application::setServerList (Server_instance *list)
{
    this->server_list = list;
}



void Http_application::allocate_servers (void)
{
    try
    {
        this->server_list = new Server_instance[this->server_count];
    }
    catch (std::bad_alloc)
    {
        throw Fatal_error ();
    }

}

void Http_application::initServers (void)
{
    std::string serv_names[3] ;
    int init_port = 1234;
    int ports[3];
    
    serv_names[0] = "serv1", ports[0] = 1200;
    serv_names[1] = "serv2", ports[1] = 8080;
    serv_names[2] = "serv3", ports[2] = 5000; 
    for (int i = 0; i < 3; i++)
    {
        this->server_list[i].setServerName (serv_names[i]);
        this->server_list[i].setServerPort (ports[i]);
    }
}

void Http_application::handleHttpRequest (int client_indx)
{
    return_value = recv (fd_pool[client_indx].fd, buffer, HEADER_MAX, 0);
    if (return_value == 0)
        std::cout << "remote peer closed connection" << std::endl;
    else if (return_value == -1)
        std::cerr << "\e[0;31mfailed to recieve data\e[0m" << std::endl;
    else
    {
        std::cout << "\e[0;32m recieved data : \e[0m" << std::endl;
        std::cout << buffer << std::endl;
    }
    // for testing and observability
    return_value = send (fd_pool[client_indx].fd, HTTP_RESPONSE_EXAMPLE, strlen (HTTP_RESPONSE_EXAMPLE), 0);
    log_file << "\e[0;33mbyte sent : \e[0m" << return_value << " \e[0;33mexpected : \e[0m" <<  strlen (HTTP_RESPONSE_EXAMPLE) << std::endl;
    return_value = close (fd_pool[client_indx].fd);
    if (return_value == -1)
        std::cerr << "\e[0;31m server failed to close connection" << std::endl;
    fd_pool[client_indx].fd = -1;
    fd_pool[client_indx].events = -1;
    indx--;
}

int Http_application::getConnectionIndx (void)
{
    return (indx);
}