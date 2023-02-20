/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelatman <yelatman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:38:00 by yelatman          #+#    #+#             */
/*   Updated: 2023/02/20 20:02:30 by yelatman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	status  = 0;
	lastRead = 0 ;
	headersSent = 0;
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
	status = 0;
	lastRead = 0 ;
	headersSent = 0;
}

Connection::~Connection()
{
	std::cout << "\e[0;31m CONNECTION CLOSED \e[0m" << std::endl;
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

int Connection::getStatus (void) const
{
	return (this->status);	
}


size_t Connection::getLastRead (void)
{
	return (lastRead);	
}

int Connection::getPeerPort (void) const
{
	return (this->port);
}

std::string Connection::getPeerAddr (void) const
{
	return (this->ipAddress);
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



int Connection::recieveData (int *start, int *len)
{
	memset (httpBuffer, 0, BUFFER_MAX + 1);
    dataRecievedLength = recv (ConnectionSocket, httpBuffer, BUFFER_MAX, 0);
	lastRead = timeInMilliseconds ();
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

void Connection::setPeerAddress (void)
{
	socklen_t len = sizeof (address);
	char 				ipStr[INET_ADDRSTRLEN];

	getsockname (this->ConnectionSocket, (sockaddr *) &address, &len);
	inet_ntop (AF_INET, &address.sin_addr, ipStr, INET6_ADDRSTRLEN);
	ipAddress = std::string(ipStr);
	port = ntohs (address.sin_port);
}

void Connection::generateResolversList (serverBlocks serverBlockList)
{
	serverBlocks::iterator it;
	int confPort = 0;
	std::string confHost;
	int serverIndx = 0;
	
	for (it = serverBlockList.begin(); it != serverBlockList.end (); it++)
	{
		confPort = (*it)->getPort ();
		confHost = (*it)->getHost();	
		if (!confHost.compare (ipAddress) && confPort == port)
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
	std::string responseHeaders;
	int responseLength = 0;
	int dataSent = 0;

	if (cgi == true)
		responseData = CGI->execute ();
	else
		responseData = response->getResponse();
	responseLength = responseData.length();
	std::cout << "response length: " << responseLength << std::endl;
	dataSent = send (fd, responseData.c_str() + responseIndex, responseLength - bytesSent, 0);
	if (dataSent < 0)
		return (-1);
	bytesSent += dataSent;
	if (bytesSent == responseLength)
		return (1);
	responseIndex = bytesSent ;		
	std::cout << "response Index: " << responseIndex << std::endl;
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

void HttpApplication::checkConnectionTimeOut (SOCKET fd)
{
	connectionPool::iterator it;
	Connection *connection;
	size_t t;
	size_t timePased;
	SOCKET connSocket;
	Response 	*response;
	Request 	*request;
	std::string resData;
	int resLen  = 0;
	int sendRet = 0;
	

	t = timeInMilliseconds ();
	it = getConnection (fd);
	if (it == connections.end ())
	{
		throw Connection_error ("CONNECTION", "CONNECTION NOT FOUND");
		return ;
	}
	connection = (*it);
	timePased  = t - connection->getLastRead ();
	connSocket =  connection->getConnectionSocket ();
	if (timePased >= REQUEST_TIMEOUT)
	{
		request = new Request (TIME_OUT);
		response = new Response (*request, config);
		resData = response->getResponse();
		resLen = resData.size ();
		sendRet = send (connection->getConnectionSocket(), resData.c_str (), resLen, 0);
		if (sendRet <= 0)
			throw Connection_error ("SEND ERROR", "SEND FAILURE");
		terminateConnection (connSocket, connection->getPeerAddr(), connection->getPeerPort ());
	}
}