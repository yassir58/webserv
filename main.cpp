#include "Server_instance.hpp"


int main (int argc , char *argv[])
{
    (void)argc;
    (void)argv;
    Http_application app (4);
    
    
    try
    {
        app.allocate_servers ();
        app.initServers ();
        app.connectServers ();
        while (1)
        {
            app.checkReadySockets ();
            // for (int i = 0; i < app.getConnectionCount (); i++)
            // {
            //     app.handleReadyConnection (i);
            // }
        }
    }
    catch (std::exception &ex) 
    {
        std::cout << ex.what () << std::endl;
    }
    return (0);
}