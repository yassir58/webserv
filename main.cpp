#include "Server_instance.hpp"
#include "config/config.hpp"

int main (int argc , char *argv[])
{
    Config *configFile ;
    if (argc == 1)
    {
        try
        {
            configFile = new Config ("./default.conf");
        }
        catch (std::exception &ex) 
        {
            std::cout << ex.what () << std::endl;
        }

    }
    else 
    {
        try
        {
            configFile = new Config (argv[argc -1]);
        }
        catch (std::exception &ex) 
        {
            std::cout << ex.what () << std::endl;
        }
    }
    try
    {
        configFile->parseConfig ();
        std::vector<Server*> servList = configFile->getHttpContext ()->getServers ();
        Http_application app (servList);
        app.allocate_servers ();
        app.initServers (configFile->getHttpContext()->getServers ());
        app.connectServers ();
        while (1)
        {
            app.checkReadySockets ();
        }
    }
    catch (std::exception &ex) 
    {
        std::cout << ex.what () << std::endl;
    }

    delete configFile ;    
    
    return (0);
}