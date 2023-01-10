#include "ServerInstance.hpp"
#include "../config/config.hpp"

int main (int argc , char *argv[])
{
    Http_application app;

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
        app.setupAppResources ();
        app.allocateServers ();
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