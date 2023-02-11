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
	requestString = "";
	ContentLength = -1;
	headerLength = -1;
	upload = -1;
	bodyRead = 0;
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
}


Connection::~Connection()
{
	//delete this->request;
}

int Connection::recieveData (int *start, int *len)
{
	httpBuffer = (char *) malloc (sizeof (char) * (BUFFER_MAX));
    dataRecievedLength = recv (ConnectionSocket, httpBuffer, BUFFER_MAX - 1, 0);
	std::cout << "data recieved in bytes \e[0;32m"  << dataRecievedLength << " \e[0m" << std::endl; 
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

			std::cout << "upload : " << upload << std::endl;
			size_t headerLen = tmpBuffer.find (CRLF);
			if (headerLen != std::string::npos)
			{
				headerLength = headerLen  + strlen (CRLF);
				*start = headerLength;
				*len  = dataRecievedLength - headerLength;
				requestHeader = tmpBuffer.substr (0, headerLength);
				std::cout << "Header Length : " << headerLength << std::endl;
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
					std::cout << "Content Length : " << ContentLength << std::endl;
					std::cout << "Body read : " << bodyRead << std::endl;
				}
			}
		}
		else
		{
			bodyRead += dataRecievedLength;
			*start = 0;
			*len = dataRecievedLength;
			std::cout << "Body read : " << bodyRead << std::endl;
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

void Connection::setRequest (serverBlocks serverList)
{
	try
    {
		std::cout << "\e[0;31m request string length: \e[0m" << std::endl;
		std::cout << requestString.length () << std::endl;
		// std::cout << "request string" <<  requestString << std::endl;
    	request = new Request (requestHeader, serverList, resolversList, requestData);
		std::cout << "wa status code: " << request->getStatusCode() << std::endl;
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

void Connection::appendBuffer (size_t start, int dataRecived)
{
	int i = start;

	std::cout << "start : " << start << std::endl;
	while (dataRecived--)
	{
		requestData.push_back(httpBuffer[i]);
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

std::vector <char> Connection::getRequestData (void) const
{
	return (this->requestData);
}

std::string Connection::getRequestHeader (void) const
{
	return (this->requestHeader);
}