#include "ServerInstance.hpp"
#include "../request/Request.hpp"
#include "../response/Response.hpp"
#include "../CGI/CGI.hpp"

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
	requestString = "";
	ContentLength = -1;
	headerLength = -1;
	upload = -1;
	bodyRead = 0;
	bytesSent = 0;
	responseConstructed = false ;
	responseIndex = 0;
}

Connection::Connection (int fd)
{
	
    ConnectionSocket = fd;
	ConnectionPort = 0;
	requestLength = 0;
	requestString = "";
	ContentLength = -1;
	headerLength = -1;
	upload = -1;
	bodyRead = 0;
	bytesSent = 0;
	responseConstructed = false ;
	responseIndex = 0;
}


Connection::~Connection()
{
	//delete this->request;
}

int Connection::recieveData (int *start, int *len)
{
	httpBuffer = (char *) malloc (sizeof (char) * (BUFFER_MAX));
    dataRecievedLength = recv (ConnectionSocket, httpBuffer, BUFFER_MAX - 1, 0);
	if (dataRecievedLength > 0)
	{
		requestLength += dataRecievedLength ;
		httpBuffer[dataRecievedLength] = 0;

		if (ContentLength == -1 && headerLength == -1)
		{
			std::string tmpBuffer (httpBuffer);
			std::string pattern ("Content-Length");
			size_t method = tmpBuffer.find ("GET");
			
			if (method == std::string::npos)
			{
				method = tmpBuffer.find ("POST");
				if (method == 0)
					upload = 1;
			}
			else if (method == 0)
				upload = 0;

			size_t headerLen = tmpBuffer.find (CRLF);
			if (headerLen != std::string::npos)
			{
				headerLength = headerLen  + strlen (CRLF);
				*start = headerLength;
				*len  = dataRecievedLength - headerLength;
				requestHeader = tmpBuffer.substr (0, headerLength);
			}
			// else
			// 	upload = -1;

			if (upload == 1)
			{
				size_t cn = tmpBuffer.find (pattern);

				if (cn != std::string::npos)
				{
					char tmp[1000];
					int i = cn + pattern.length () + 1;
					int j = 0;

					while (tmpBuffer[i] && tmpBuffer[i] != '\n')
					{
						tmp[j] =tmpBuffer[i];
						i++;
						j++;
					}
					tmp[j] = 0;
					ContentLength = atoi (tmp);
					bodyRead = dataRecievedLength - headerLength;
					// std::cout << "Content Length : " << ContentLength << std::endl;
					// std::cout << "Body read : " << bodyRead << std::endl;
				}
			}
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

void Connection::setRequest (void)
{
	try
    {
		std::cout << requestString.length () << std::endl;
    	request = new Request (this);
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

void Connection::appendBuffer (size_t start, int dataRecived)
{
	int i = start;

	while (dataRecived--)
	{
		requestBody.push_back(httpBuffer[i]);
		i++;
	}
}

std::string Connection::getRequestString (void) const
{
	return (this->requestString);
}


// void Connection::appendToBinaryFile (size_t n)
// {
// 	file.write (requestString.c_str (), n);
// }

int Connection::getDataToRead(void) const
{
	return (this->dataReminder);
}


void Connection::setDataTorRead (int dataLength)
{
	this->dataToRead = dataLength;
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


void Connection::setServerBlocks (serverBlocks serverList) 
{
	this->servList  = serverList;
}

void Connection::setConfig (Config *conf)
{
	this->conf  = conf;
}

Config *Connection::getConfig (void) const
{
	return (this->conf);	
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
	// std::cout << "-----------------------------------------------------------\n";
	std::cout << "\e[0;33m" << responseIndex << "\e[0m]" <<std::endl;
	dataSent = send (fd, responseData.c_str () + responseIndex, responseLength - bytesSent, 0);
	std::cout << "\e[0;31m Bytes sent " << dataSent << "\e[0m" << std::endl;
	std::cout << "\e[0;31m fd " << fd << "\e[0m" << std::endl;
	std::cout << "\e[0;31m connectionFd " << ConnectionSocket << "\e[0m" << std::endl;
	if (dataSent < 0)
		return (-1);
	bytesSent += dataSent;
	std::cout << "\e[0;33m-------- bytes sent " << bytesSent << "\e[0;36m Excpected " << responseData.length ()   << "-------- \e[0m" << std::endl;
	if (bytesSent == responseLength)
		return (1);
	responseIndex = bytesSent ;
	return (0);
}

void Connection::constructResponse (void)
{
	if (responseConstructed == false)
	{
		if (request->getLocation () && !request->getLocation()->getEndPoint().empty())
		{
			std::cout << "\e[0;31m PATH : \e[0m" << request->getPath () << std::endl;
			if (request->getCGIStatus())
				std::cout << "CGI status: Enabled" << std::endl;
			else
				std::cout << "CGI status: Disabled" << std::endl;
		}
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