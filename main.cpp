#include "Server_instance.hpp"




int main (int argc , char *argv[])
{
    (void)argc;
    (void)argv;
    int err;
    Server_instance *servers;
    pollfd fd_pool[MAX_CONNECT];;
    Http_application app (3);
    int indx = 3;
    int gb_connec_count = 0;
    std::ofstream log_file;
    int ret = 0;
    char buff[1024];
    int i = 0;
    static std::string rsp[] = { 
    "HTTP/1.1 200 OK\r\n",
    "Server: WebServer\r\n",
    "Content-Type: text/html\r\n",
    "Content-Length: 3\r\n",
    "Connection: close\r\n",
    "\r\n",
    "123"
};

    std::string test = "HTTP/1.1 200 OK\r\nServer: WebServer\r\nContent-Type: text/html\r\nContent-Length: 11\r\nConnection: close\r\n\r\nhello world";

  

    servers = new Server_instance[3];
    log_file.open ("server_log", std::ios_base::app);
    initServers (servers);
    try
    {
        for (int i = 0; i <  3 ; i++)
        {
            servers[i].establish_connection ();
            fd_pool[i].fd = servers[i].getSocketFd ();
            fd_pool[i].events = POLLIN;
        }
        while (1)
        {
            err = poll (fd_pool,MAX_CONNECT, POLL_TIMEOUT);
            if (err == 0)
                std::cout << "\e[0;31m connection timeout \e[0m" << std::endl;
            if (err < 0)
                handle_error (POLLERROR);
            for (int i = 0; i < indx; i++)
            {
                if (fd_pool[i].revents == POLLIN)
                {
                    if (i >= app.getServerRangeStart () && i < app.getServerRangeEnd ())
                    {
                        std::cout << i << app.getServerRangeStart () << app.getServerRangeEnd () <<  std::endl;
                        fd_pool[indx].fd = servers[i].accept_connection();
                        fd_pool[indx].events = POLLIN;
                        indx++;
                        log_file << "connection count : "<< gb_connec_count << std::endl;
                        log_file <<  "\e[0;32mserver \e[0m: " << servers[i].getServerName () << " " << currentDateTime ()  <<  std::endl;
                        gb_connec_count++;
                    }
                    else
                    {
                        //app.handleReadyConnection (fd_pool[i].fd);
                        ret = recv (fd_pool[i].fd, buff, HEADER_MAX, 0);
                        if (ret == 0)
                            std::cout << "remote peer closed connection" << std::endl;
                        else if (ret == -1)
                            std::cerr << "\e[0;31mfailed to recieve data\e[0m" << std::endl;
                        else
                        {
                            std::cout << "\e[0;32m recieved data : \e[0m" << std::endl;
                            std::cout << buff << std::endl;
                        }
                        // for (int i = 0; i < RESPONSE_LINES; i++)
                        // {
                        //     ret = send (fd_pool[i].fd, rsp[i].c_str (), RESPONSE_LINES, 0);
                        //     if (ret == -1)
                        //         std::cerr << "\e[0;31mfailed to send data\e[0m" << std::endl;
                        //     else if (ret != rsp[i].length ())
                        //         std::cerr << "\e[0;31mdata are not complete\e[0m" << std::endl;
                        //     else if (ret == 0)
                        //         std::cerr << "e[0;31mserver shutdown\e[0m" << std::endl;
                        //     std::cout << "ret : " << ret << std::endl;

                        // }
                        ret = send (fd_pool[i].fd, test.c_str (), 124, 0);
                        std::cout << "byte sent : " << ret << "expected" << test.length () << std::endl;
                        ret = close (fd_pool[i].fd);
                        if (ret == -1)
                            std::cerr << "\e[0;31m server failed to close connection" << std::endl;
                        fd_pool[i].fd = -1;
                        fd_pool[i].events = -1;
                        indx--;
                    }
                }
            }
           
        }
    }
    catch (std::exception &ex) 
    {
        std::cout << ex.what () << std::endl;
        delete [] servers;
        return (EXIT_FAILURE);
    }
    delete [] servers;
    log_file.close ();
    return (0);
}