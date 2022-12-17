#include "Server_instance.hpp"




int main (int argc , char *argv[])
{
    (void)argc;
    (void)argv;
    //int err;

    try
    {
        Server_instance main_server(8080, "main server");

        main_server.bind_socket ();
        main_server.establish_connection ();
    }
    catch (std::exception &ex) 
    {
        std::cout << ex.what () << std::endl;
        return (EXIT_FAILURE);
    }

    try
    {
        Server_instance another_server(1234, "ras lavoca");

        another_server.bind_socket ();
        another_server.establish_connection ();
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what () << std::endl;
        return (EXIT_FAILURE);
    }
    return (0);
}