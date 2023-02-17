#include "ServerInstance.hpp"
#include "../request/Request.hpp"
#include "../response/Response.hpp"
#include "../CGI/CGI.hpp"


// * *  ------------------------------------------- CONSTRUCTORS -------------------------------------------  ** //

Connection::Connection ()
{
    ConnectionSocket = 0;
	ConnectionPort = 0;
	requestLength = 0;
	ContentLength = -1;
	headerLength = -1;
	upload = -1;
	bodyRead = 0;
	bytesSent = 0;
	responseConstructed = false ;
	responseIndex = 0;
	request = NULL;
	response = NULL;
	CGI = NULL ;
}

Connection::Connection (int fd)
{
	
    ConnectionSocket = fd;
	ConnectionPort = 0;
	requestLength = 0;
	ContentLength = -1;
	headerLength = -1;
	upload = -1;
	bodyRead = 0;
	bytesSent = 0;
	responseConstructed = false ;
	responseIndex = 0;
	request = NULL;
	response = NULL;
	CGI = NULL ;
}

Connection::~Connection()
{
	if (this->request)
		delete this->request;
	if (this->response)
		delete this->response ;
	if (this->CGI)
		delete this->CGI;
}

// * *  ------------------------------------------- GETTERS -------------------------------------------  ** //

Config *Connection::getConfig (void) const
{
	return (this->conf);	
}

Request *Connection::getRequest (void) const
{
	return (this->request);
}

serverBlocks Connection::getServerBlocks (void) const
{
	return (this->servList);
}

size_t Connection::getBytesSent (void) const 
{
	return (this->bytesSent);
}
bool Connection::getResponseState (void) const
{
	return (this->responseConstructed);
}

int Connection::getRequestLength (void) const
{
	return (this->requestLength);
}

int Connection::getBodyRead (void) const
{
	return (bodyRead);	
}

int Connection::getUpload (void) const
{
	return (upload);
}

int Connection::getContentLength (void) const
{
	return (ContentLength);
}

std::vector <char> Connection::getRequestBody (void) const
{
	return (this->requestBody);
}

std::string Connection::getRequestHeaders (void) const
{
	return (this->requestHeader);
}

// * *  ------------------------------------------- SETTERS -------------------------------------------  ** //

void Connection::setRequest (void)
{
    request = new Request (this);
}

void Connection::setServerBlocks (serverBlocks serverList) 
{
	this->servList  = serverList;
}

void Connection::setConfig (Config *conf)
{
	this->conf  = conf;
}

// * *  ------------------------------------------- HELPERS -------------------------------------------  ** //

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
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

int Connection::recieveData (int *start, int *len)
{
	memset (httpBuffer, 0, BUFFER_MAX + 1);
    dataRecievedLength = recv (ConnectionSocket, httpBuffer, BUFFER_MAX, 0);
	if (dataRecievedLength > 0)
	{
		requestLength += dataRecievedLength ;
		if (ContentLength == -1 && headerLength == -1)
		{
			std::string tmpBuffer (httpBuffer);
			
			extractMethod (tmpBuffer);
			extractHeaderLength (tmpBuffer, start, len);
			if (upload == 1)
				extractContentLength (tmpBuffer);
		}
		else
		{
			bodyRead += dataRecievedLength;
			*start = 0;
			*len = dataRecievedLength;
		}				
	}
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
		if (!serverList[(*it)]->getServerName ().compare(servName))
			servIndx = (*it);
	}
	return (serverList[servIndx]);
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

void HttpApplication::handleSigPipe (void) 
{
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGPIPE, &sa, NULL);
}

void Connection::appendBuffer (size_t start, int dataRecived)
{
	int i = start;

	while (dataRecived--)
	{
		requestBody.push_back(httpBuffer[i]);
		i++;
	}
}


int Connection::sendResponse (int fd)
{
	std::string responseData;
	int responseLength = 0;
	int dataSent = 0;

	if (cgi == true)
		responseData = CGI->execute ();
	else
		responseData = response->getResponse ();
	responseLength = responseData.length();
	dataSent = send (fd, responseData.c_str () + responseIndex, responseLength - bytesSent, 0);
	if (dataSent < 0)
		return (-1);
	bytesSent += dataSent;
	if (bytesSent == responseLength)
		return (1);
	responseIndex = bytesSent ;
	return (0);
}

void Connection::constructResponse (void)
{
	if (responseConstructed == false)
	{
		if (request->getCGIStatus())
		{
			cgi = true ;
			CGI = new CGIHandler (request);
			responseConstructed = true ;
		}
		else 
		{
			cgi = false;
			response = new Response (*request, conf);
			responseConstructed = true ;
		}
	}
}



