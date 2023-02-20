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
    catch (Parse_error &err)
    {
        std::cout << err.what () << std::endl;
        exit (EXIT_FAILURE);
    }
    catch (std::exception &exc)
    {
        std::cout << exc.what () << std::endl;
        app.deleteConfig ();
        system ("leaks webserv");
        exit (EXIT_FAILURE);
    }
    
    try
    {
		app.handleSigPipe ();
        app.filterServerBlocks ();
        app.connectServers ();
		app.initServerSet ();
        while (1)
        {
            try 
            {
                app.checkForConnection ();
            }
            catch (Fatal_error &exc)
            {
                std::cout << exc.what () << std::endl;
                app.deleteConfig ();
                app.deleteServers ();
                app.deleteConnections ();
                exit (EXIT_FAILURE);
            }
            catch (std::exception &exc)
            {
                 std::cout << exc.what () << std::endl;
				 // handle error
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