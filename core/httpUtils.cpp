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
	clientPort = 0;
	requestLength = 0;
	requestHandlerIndx = 0;
}

Client::Client (int fd)
{
    clientSocket = fd;
	clientPort = 0;
	requestLength = 0;
	requestHandlerIndx = 0;
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
		requestLength = dataRecievedLength;
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
	int confPort = 0;
	std::string confHost;
	int serverIndx = 0;
	struct sockaddr_in address;
	socklen_t len = sizeof (address);
	char ipStr[INET_ADDRSTRLEN];
	int port;
	
	
	getsockname (this->clientSocket,(sockaddr *) &address, &len);
	inet_ntop (AF_INET, &address.sin_addr, ipStr, INET6_ADDRSTRLEN);
	port = ntohs (address.sin_port);
	for (it = serverBlockList.begin(); it != serverBlockList.end (); it++)
	{
		confPort = (*it)->getPort ();
		confHost = (*it)->getHost();	
		if (!confHost.compare (std::string(ipStr)) && confPort == port)
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

int Client::matchRequestHandler (serverBlocks serverList)
{
	int servIndx = 0;
	int hostFlag = this->request->getStartLine().Host;
	std::string servName;
	intContainer::iterator it;

	servName = this->request->getStartLine().hostName ;
	if (hostFlag)
	{
		for (it = resolversList.begin (); it != resolversList.end (); it++)
		{
			servIndx = (*it);
			if (!serverList[(*it)]->getServerName ().compare(servName))
				return (*it);
		}
	}
	return (0);
}

int Client::getHandlerIndx (void) const
{
	return  (this->requestHandlerIndx);
}