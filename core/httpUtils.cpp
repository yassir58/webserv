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


Client::~Client()
{
	delete this->request;
}

void Client::recieveData (void)
{
    dataRecievedLength = recv (clientSocket, buffer, BUFFER_MAX, 0);
    if (dataRecievedLength == 0)
        std::cout << "\e[0;31m remote peer closed connection\e[0m" << std::endl;
    else if (dataRecievedLength == -1)
        throw Connection_error (strerror (errno), "recv");
    else
    {
        std::cout << dataRecievedLength << "\e[0;37m byte recieved\e[0m" << std::endl;
    }
}


int HttpApplication::isServer (int fd)
{
	std::vector <int>::iterator it;


	for (it = serverFds.begin(); it != serverFds.end (); it++)
	{
		if (*it == fd)
			return (TRUE);
	}
	return (FALSE);
}

void Client::generateResolversList (serverBlocks serverBlockList)
{
	serverBlocks::iterator it;
	std::string service;
	std::string host;
	int serverIndx = 0;
	
	this->ipAddress = request->getStartLine().IpAdress;
	this->serviceName = request->getStartLine().Port;
	this->hostName = request->getStartLine().hostName;
	std::cout << "host check " << this->request->getStartLine().Host << std::endl;
	std::cout << this->request->getStartLine().hostName << std::endl;
	for (it = serverBlockList.begin(); it != serverBlockList.end (); it++)
	{
		service = int2assci ((*it)->getPort ());
		hostName = (*it)->getHost();
		//if (this->request->getStartLine().Host)
		if (!this->ipAddress.compare(hostName) && !this->serviceName.compare(service))
			this->resolversList.push_back (serverIndx);
		serverIndx++;
	}
}

void Client::setRequest (void)
{
	try
    {
    	request = new Request (buffer);
    }
    catch (std::exception &exc)
    {
        std::cout << "\e[0;31m" << exc.what () << "\e[0m" << std::endl;
    }
}

void Client::printfResolvers (void)
{
	std::vector<int>::iterator it;

	for (it = resolversList.begin (); it != resolversList.end (); it++)
	{
		std::cout << (*it) << " ";
	}
	std::cout << std::endl;
}

void Client::matchRequestHandler (serverBlocks serverList)
{
	std::vector<int>::iterator it ;

	std::cout << "request host" <<  this->request->getStartLine().hostName << std::endl;
	for (it = this->resolversList.begin(); it != this->resolversList.end (); it++)
	{
		std::cout << "config host" <<  serverList[(*it)]->getServerName () << std::endl;
	}
}