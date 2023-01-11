#include "ServerInstance.hpp"
#include "../request/Request.hpp"

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}


void HttpApplication::serverLog (int serverIndx)
{
    logFile << "connection count : "<< connectionCount  << " ";
    // logFile <<  "\e[0;32mserver name: \e[0m: " <<     serverList[serverIndx]->getServerName () << " ";
    logFile << "\e[0;32mserver port: \e[0m:" << serverList[serverIndx]->getServerPort () << " ";
    logFile << " " << currentDateTime ()  <<  std::endl;
}

void handleError (int err)
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
        break;
        case WRITEERR:
            std::cerr << "\e[0;31mcontext : \e[0m" << "write" << std::endl;
        break;
        case CLOSEERR:
            std::cerr << "\e[0;31mcontext : \e[0m" << "close" << std::endl;
        break;
        default:
             std::cerr << "\e[0;31mcontext : \e[0m" << "close" << std::endl;
            break;
    }
    throw Connection_error (strerror (errno), "uknown error");
}


Client::Client ()
{
    clientSocket = 0;
}

Client::Client (int fd)
{
    clientSocket = fd;
}

void Client::recieveData (void)
{
    dataRecievedLength = recv (clientSocket, buffer, BUFFER_MAX, 0);
    if (dataRecievedLength == 0)
        std::cout << "remote peer closed connection" << std::endl;
    else if (dataRecievedLength == -1)
        throw Connection_error (strerror (errno), "recv");
    else
    {
        std::cout << dataRecievedLength << "\e[0;32m byte recieved\e[0m" << std::endl;
        try
        {
            request = new Request (buffer);
            //request->printResult ();
            std::cout << "Host Name" << request->getStartLine ().hostName << std::endl;
            std::cout << "Ip Address" << request->getStartLine ().IpAdress << std::endl;
            std::cout << "Port Number" << request->getStartLine ().Port << std::endl;
        }
        catch (std::exception &exc)
        {
            std::cout << "\e[0;31m" << exc.what () << "\e[0m" << std::endl;
        }
        //delete request;
    }
}
