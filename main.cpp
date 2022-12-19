#include "Server_instance.hpp"




int main (int argc , char *argv[])
{
    (void)argc;
    (void)argv;
    //int err;
    Server_instance main_server(8080, "main server");
    Server_instance another_sr(1242, "another");
    nfds_t _fds_num = MAX_CONNECT;
    try
    {
        struct pollfd connection_poll[1024];
        int connection_count = 2;
        int err;
        int indx = connection_count;
        char buffer [HEADER_MAX];
        int ret;


        bzero (buffer, HEADER_MAX);
        main_server.establish_connection ();
        another_sr.establish_connection ();
        while (1)
        {
            connection_poll[0].fd = main_server.getSocketFd ();
            connection_poll[0].events = POLLIN;
            connection_poll[1].fd = another_sr.getSocketFd ();
            connection_poll[1].events = POLLIN;
            err = poll (connection_poll,1024, -1);
            if (err == 0)
                throw Connection_error ("TIMEOUT");
            else if (err < 0)
                throw Connection_error (strerror (errno));
            for (int i = 0; i < 1024; i++)
            {
                if (connection_poll[i].revents == POLLIN)
                {

                    if (connection_poll[i].fd == main_server.getSocketFd ())
                    {
                        /// accept connection for server1
                        connection_poll[indx].fd = main_server.accept_connection ();
                        connection_poll[indx].events = POLLIN;
                        indx++;
                    }
                    else if (connection_poll[i].fd == another_sr.getSocketFd ())
                    {
                        /// accept connections for server2
                        connection_poll[indx].fd = another_sr.accept_connection ();
                        connection_poll[indx].events = POLLIN;
                        indx++;
                    }
                    else
                    {
                        /// handle request
                        ret = read(connection_poll[i].fd, buffer, HEADER_MAX);
                        if (ret == -1)
                        {
                            std::cout << strerror (errno) << std::endl;
                            throw Connection_error ("read error");
                        }
                        if (ret == 0)
                            std::cerr << "\e[0;31m NOTHING TO READ \e[0m" << std::endl;
                        std::cout << "\e[0;33m message sent by client :\e[0m" << std::endl;
                        std::cout << buffer << std::endl;
                        send (connection_poll[i].fd, ACK_MESSAGE, strlen (ACK_MESSAGE), 0);
                        close (connection_poll[i].fd);
                        connection_poll[i].fd = -1;
                        connection_poll[i].events = -1;
                        indx--;
                        bzero (buffer, HEADER_MAX);
                    }
                }
            }
            std::cout << "server 1 : " << main_server.getRequestCount () << std::endl;
            std::cout << "server 2 : " << another_sr.getRequestCount () << std::endl;
        }
    }
    catch (std::exception &ex) 
    {
        std::cout << ex.what () << std::endl;
        return (EXIT_FAILURE);
    }
    

    return (0);
}