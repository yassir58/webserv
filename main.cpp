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

  

    servers = new Server_instance[3];
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
            err = poll (fd_pool,MAX_CONNECT, POLL_INF);
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
                    }
                    else
                    {
                        app.handleReadyConnection (fd_pool[i].fd);
                        fd_pool[i].fd = -1;
                        fd_pool[i].events = -1;
                        indx--;
                    }
                }
            }
            std::cout << "connection count : "<< indx << std::endl;
        }
    }
    catch (std::exception &ex) 
    {
        std::cout << ex.what () << std::endl;
        delete [] servers;
        return (EXIT_FAILURE);
    }
    delete [] servers;
    return (0);
}