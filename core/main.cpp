#include "ServerInstance.hpp"
#include "../config/config.hpp"
#include "../request/request.h"
#include "../request/Request.hpp"

int main (int argc , char *argv[])
{
    HttpApplication app;

    try 
    {
        app.handleConfig (argc, argv);
    }
    catch (std::exception &exc)
    {
        std::cout << exc.what () << std::endl;
        exit (EXIT_FAILURE);
    }
    
    try
    {
        // app.filterServerBlocks ();
        // app.setupAppResources ();
        // app.connectServers ();
        // serverContainer servList = app.getServerList ();
        // for (serverContainer::iterator it = servList.begin (); it < servList.end (); it++)
        // {
        //         std::cout << "Host : " << (*it)->getHostName () << std::endl;
        //         std::cout << "Port : " << (*it)->getServerPort () << std::endl;
        // }
        // while (1);
        app.setupAppResources ();
        app.filterServerBlocks ();
        app.connectServers ();
        while (1)
        {
            try 
            {
                app.checkForConnection ();
            }
            catch (Fatal_error &exc)
            {
                std::cout << exc.what () << std::endl;
                exit (EXIT_FAILURE);
            }
            catch (std::exception &exc)
            {
                 std::cout << exc.what () << std::endl;
                 exit (EXIT_FAILURE);
            }
        }    
    }
    catch (std::exception &ex) 
    {
        std::cout << ex.what () << std::endl;
        exit (EXIT_FAILURE);
    }
    return (0);
}