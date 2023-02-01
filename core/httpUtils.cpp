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
    accessLog <<  "\e[0;36mserver interface : \e[0m: " <<     serverList[serverIndx]->getHostName() << " ";
    accessLog << "\e[0;32mserver port: \e[0m:" << serverList[serverIndx]->getServerPort () << " ";
    accessLog << " \e[0;33m" << currentDateTime ()  << "" <<  std::endl;
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


Connection::Connection ()
{
    ConnectionSocket = 0;
	ConnectionPort = 0;
	requestLength = 0;
}

Connection::Connection (int fd)
{
    ConnectionSocket = fd;
	ConnectionPort = 0;
	requestLength = 0;
}


Connection::~Connection()
{
	//delete this->request;
}

int Connection::recieveData (void)
{
    dataRecievedLength = recv (ConnectionSocket, httpBuffer, BUFFER_MAX, 0);
	return (dataRecievedLength);
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

void Connection::generateResolversList (serverBlocks serverBlockList)
{
	serverBlocks::iterator it;
	int confPort = 0;
	std::string confHost;
	int serverIndx = 0;
	struct sockaddr_in address;
	socklen_t len = sizeof (address);
	char ipStr[INET_ADDRSTRLEN];
	int port;
	
	
	getsockname (this->ConnectionSocket,(sockaddr *) &address, &len);
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

void Connection::setRequest (serverBlocks serverList)
{
	try
    {
    	request = new Request (httpBuffer, serverList, resolversList);
    }
    catch (std::exception &exc)
    {
        std::cout << "\e[0;31m" << exc.what () << "\e[0m" << std::endl;
    }
}

void Connection::printfResolvers (void)
{
	std::vector<int>::iterator it;

	for (it = resolversList.begin (); it != resolversList.end (); it++)
	{
		std::cout << (*it) << " ";
	}
	std::cout << std::endl;
}

Server *Request::matchRequestHandler (serverBlocks serverList, std::vector <int> resolversList)
{
	int servIndx = 0;
	int hostFlag = this->getStartLine().Host;
	std::string servName;
	intContainer::iterator it;

	servIndx = resolversList[0];
	servName = this->getStartLine().hostName ;
	
	for (it = resolversList.begin (); it != resolversList.end (); it++)
	{
		std::cout << "Host : " << servName << std::endl;
		std::cout << "servName : " << serverList[(*it)]->getServerName () << std::endl;
		if (!serverList[(*it)]->getServerName ().compare(servName))
			servIndx = (*it);
	}
	std::cout << "\e[0;31m server index \e[0m" << servIndx << std::endl;
	return (serverList[servIndx]);
}


void Connection::sendResponse (void)
{
	// Response newResponse ((*this->request));
	// stringContainer::iterator it;
	// stringContainer response;
	// std::string body;
	// int sendReturn;

	
	// response = newResponse.getResponse ();
	// std::cout << "\e[0;31m response content \e[0m" << std::endl;
	// for (it = response.begin(); it != response.end (); it++)
	// {
	// 	send (ConnectionSocket, (*it).c_str(), (*it).length(), 0);
	// 	std::cout << (*it) << std::endl;
	// }
    // //sendReturn = send (ConnectionSocket, HTTP_RESPONSE_EXAMPLE, strlen (HTTP_RESPONSE_EXAMPLE), 0);
	// body = getTestBody ("./testing/indx.html");
	// sendReturn = send (ConnectionSocket, body.c_str (), body.length(), 0);
	// if (sendReturn == -1)
	// 	throw Connection_error (strerror (errno), "SEND");
	// else if (sendReturn == 0)
	// 	std::cout << "\e[0;31m server closed connection \e[0m" << std::endl;
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


void HttpApplication::connectionAccessLog (std::string msg, int requestLength, std::string addr, std::string port)
{
	accessLog << currentDateTime () << " " ;
	accessLog << "\e[0;36m Peer Address : \e[0m" << addr << " ";
	accessLog << "\e[0;32m Connection Port : \e[0m" << port << " ";
	accessLog << "\e[0;33m status : \e[0m" << requestLength << " Bytes " << msg;
	accessLog << std::endl;
}

void HttpApplication::connectionErrorLog (std::string errorContext, std::string errorMessage, std::string addr, std::string port)
{
	accessLog << currentDateTime () << " " ;
	accessLog << "\e[0;36m Peer Address : \e[0m" << addr << " ";
	accessLog << "\e[0;32m Connection Port : \e[0m" << port << " ";
	errorLog << "\e[0;31m Error Context: \e[0m" << errorContext << " ";
	errorLog << "\e[0;31m Error Context: \e[0m" << errorMessage << " ";
	errorLog << std::endl;
}

Request *Connection::getRequest (void) const
{
	return (this->request);
}


void HttpApplication::handleSigPipe (void) 
{
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGPIPE, &sa, NULL);
}

// std::string listDirectory (std::string dirPath)
// {
// 	DIR *dir = opendir (dirPath.c_str ());
// 	struct dirent *dp;
// 	stringContainer dirIndex;
// 	stringContainer::iterator it;
// 	std::string responseBody("") ;
// 	std::string styles ("<style> body { background-color: #F2F2F2;}h1 {color: #0A2647;margin-left: 40px;} a {color:#5463FF; } </style>");
// 	std::string indexHeader("<h1> Index of");
// 	std::string lineBreak ("</br>");
// 	std::string line("<hr>");
// 	std::string lsOpen ("<ul>");
// 	std::string lsClose ("</ul>");
// 	std::string liOpen ("<li>");
// 	std::string liClose ("</li>");
// 	std::string linkOpen ("<a href=\"");
// 	std::string linkClose ("</a>");
// 	std::string hrClose ("\">");
// 	std::string indexBody;
// 	std::string href ("./");

// 	if (dir == NULL)
// 		throw std::exception ();
// 	else
// 	{
// 		while ((dp = readdir (dir)))
// 		{
// 			dirIndex.push_back (dp->d_name);
// 		}
// 	}
// 	responseBody.append (styles).append(indexHeader).append (dirPath) .append("</h1>").append (lineBreak).append (line).append (lsOpen);
// 	for (it = dirIndex.begin (); it != dirIndex.end (); it++)
// 	{
// 		href.append ((*it));
// 		responseBody.append (liOpen).append (linkOpen).append(href).append (hrClose).append ((*it)).append (linkClose).append (liClose);
// 		href = "./";
// 	}
// 	responseBody.append (lsClose);
// 	std::cout << "response length" << responseBody.length () << std::endl;
// 	closedir (dir);
// 	return (responseBody);
// }
