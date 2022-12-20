#include "Server_instance.hpp"

void handle_error (int err)
{
    switch (err)
    {
        case READERR:
            std::cerr << "\e[0;31mcontext : \e[0m" << "read" << std::endl;
        break;
        case ACCEPTERR:
            std::cerr << "\e[0;31mcontext : \e[0m" << "accept" << std::endl;
        break; 
        case LISTENERR:
            std::cerr << "\e[0;31mcontext : \e[0m" << "listen" << std::endl;
        break;
        case BINDERR:
            std::cerr << "\e[0;31mcontext : \e[0m" << "bind" << std::endl;
        break;
        case POLLERR:
            std::cerr << "\e[0;31mcontext : \e[0m" << "poll" << std::endl;
        case WRITEERR:
            std::cerr << "\e[0;31mcontext : \e[0m" << "write" << std::endl;
        break;
        default:
            break;
    }
    throw Connection_error (strerror (errno));
}