#include "ServerInstance.hpp"
#include "../request/Request.hpp"
#include "../response/Response.hpp"

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
    logFile <<  "\e[0;36mserver interface : \e[0m: " <<     serverList[serverIndx]->getHostName() << " ";
    logFile << "\e[0;32mserver port: \e[0m:" << serverList[serverIndx]->getServerPort () << " ";
    logFile << " \e[0;33m" << currentDateTime ()  << "" <<  std::endl;
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

void Client::sendResponse (void)
{
	Response newResponse ((*this->request));
	stringContainer::iterator it;
	stringContainer response;
	std::string body;
	int sendReturn;

	
	response = newResponse.getResponse ();
	std::cout << "\e[0;31m response content \e[0m" << std::endl;
	for (it = response.begin(); it != response.end (); it++)
	{
		send (clientSocket, (*it).c_str(), (*it).length(), 0);
		std::cout << (*it) << std::endl;
	}
    //sendReturn = send (clientSocket, HTTP_RESPONSE_EXAMPLE, strlen (HTTP_RESPONSE_EXAMPLE), 0);
	body = getTestBody ("./testing/indx.html");
	sendReturn = send (clientSocket, body.c_str (), body.length(), 0);
	if (sendReturn == -1)
		throw Connection_error (strerror (errno), "SEND");
	else if (sendReturn == 0)
		std::cout << "\e[0;31m server closed connection \e[0m" << std::endl;
    //logFile << "\e[0;33mbyte sent : \e[0m" << sendReturn << " \e[0;33mexpected : \e[0m" <<  strlen (HTTP_RESPONSE_EXAMPLE) << std::endl;
}


std::string getTestBody (std::string filename)
{
	std::ifstream file;
	std::string result;
	std::string line;


	file.open (filename, std::ios_base::in);
	while (std::getline (file, line))
	{
		result.append (line);
		if (file.peek () != EOF)
			result.append ("\n");
	}
	file.close ();
	return (result);
}