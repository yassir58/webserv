#include "Server_instance.hpp"


Http_application::Http_application ()
{
    server_count = 0;
    connection_count = 0;
    server_range_start = 0;
    server_range_end = 0;
    client_max_body_size = POST_LIMIT;
    bzero (buffer, HEADER_MAX);
}

Http_application::Http_application (int servers_n)
{
    server_count = servers_n;
    connection_count= 0;
    server_range_start = 0;
    server_range_end = servers_n;
    client_max_body_size = POST_LIMIT;
    bzero (buffer, HEADER_MAX);
}

Http_application::~Http_application ()
{
    std::cout << "\e[0;31m HTTP APPLICATION CLOSED \e[0m" <<std::endl;
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
    for (int i = 0; i < server_count; i++)
        this->server_list[i].establish_connection ();
}


void Http_application::setConnectionPool (pollfd *fd_pool)
{
    this->connection_pool = fd_pool;
}

pollfd * Http_application::getConnectionPool (void) const
{
    return (this->connection_pool);
}

Server_instance *Http_application::getServerList (void) const
{
    return (this->server_list);
}

void Http_application::handleNewConnection (int server_indx)
{
    this->connection_pool[server_indx].fd = this->server_list[server_indx].establish_connection ();
    this->connection_pool[server_indx].events = POLLIN;
    indx++;
}

void Http_application::handleReadyConnection (int client_fd)
{
    return_value = read(client_fd, buffer, HEADER_MAX);
    if (return_value == -1)
        handle_error (READERR);
    if (return_value == 0)
        std::cerr << "\e[0;31m NOTHING TO READ \e[0m" << std::endl;
    std::cout << "\e[0;33m message sent by client :\e[0m" << std::endl;
    std::cout << buffer << std::endl;
    send (client_fd, ACK_MESSAGE, strlen (ACK_MESSAGE), 0);
    close (client_fd);
    indx--;
    bzero (buffer, HEADER_MAX);
}

void Http_application::setServerList (Server_instance *list)
{
    this->server_list = list;
}

void initServers (Server_instance *serv_list)
{
    std::string serv_names[3] ;
    int init_port = 1234;
    
    serv_names[0] = "serv1";
    serv_names[1] = "serv2";
    serv_names[2] = "serv3"; 
    for (int i = 0; i < 3; i++)
    {
        serv_list[i].setServerName (serv_names[i]);
        serv_list[i].setServerPort (init_port++);
    }
}