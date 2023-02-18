/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yelatman <yelatman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:38:33 by yelatman          #+#    #+#             */
/*   Updated: 2023/02/18 22:16:26 by yelatman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerInstance.hpp"
#include "../response/Response.hpp"

int ServerInstance::accept_connection (void)
{
    int new_connection;
    
    struct sockaddr_storage ConnectionAddr;
    socklen_t ConnectionLen = sizeof (ConnectionAddr);

    new_connection = accept (this->serverSocket, (sockaddr *)&ConnectionAddr, (socklen_t *) &ConnectionLen);
    if (new_connection < 0)
        throw Connection_error (strerror (errno), "accept");
    this->requestCount++;
    return (new_connection);
}


void ServerInstance::setServerPort (unsigned int port)
{
    this->connectionPort = port;
}

unsigned int ServerInstance::getServerPort (void)
{
    return (this->connectionPort);
}


int Connection::getConnectionSocket (void) const
{
    return (this->ConnectionSocket);
}

std::vector <int>Connection::getResolversList (void) const
{
    return (this->resolversList);
}

char *Connection::getBuffer (void) 
{
    return (this->httpBuffer);
}

void Connection::emptyBuffer (void) 
{
   free (httpBuffer);
}

void Connection::setStatus (int status)
{
	this->status = status;  
}


void HttpApplication::closeConnection (SOCKET fd, std::string  error)
{
	
	int err = 0;

	
	FD_CLR (fd, &readFds);
	FD_CLR (fd, &errorFds);
	FD_CLR (fd, &writeFds);
	err  = close (fd);
	if (err < 0)
		throw Connection_error (strerror (errno), "CLOSE");
	throw Connection_error (error.c_str (), "RECV FAILURE");
}

void HttpApplication::closeConnection (SOCKET fd)
{
	connectionPool::iterator it;

	it = getConnection (fd);
	FD_CLR (fd, &writeFds);
	FD_CLR (fd, &errorFds);
	close (fd);
	if (it == connections.end ())
		throw Connection_error ("CONNECTION ERROR", "COULD NOT FIND CONNECTION");
	connections.erase (it);
}

void HttpApplication::removeConnection (SOCKET fd)
{
	intContainer::iterator it;

	it = std::find (openConnections.begin (), openConnections.end (), fd);
	if (it != openConnections.end ())
		openConnections.erase (it);
}


void Connection::extractHeaderLength (std::string tmpBuffer, int *start, int *len)
{
	size_t headerLen = tmpBuffer.find (CRLF);
	if (headerLen != std::string::npos)
	{
		headerLength = headerLen  + strlen (CRLF);
		*start = headerLength;
		*len  = dataRecievedLength - headerLength;
		requestHeader = tmpBuffer.substr (0, headerLength);
		status++;
	}
}

void Connection::extractContentLength (std::string tmpBuffer)
{
	std::string pattern ("Content-Length");
	size_t cn = tmpBuffer.find (pattern);

	if (cn != std::string::npos)
	{
		char tmp[10000];
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
	}
}

void Connection::extractMethod (std::string tmpBuffer)
{
	size_t method = tmpBuffer.find ("GET");
			
	if (method == std::string::npos)
	{
		method = tmpBuffer.find ("POST");
		if (method == 0)
		{
			upload = 1;
			status++;
		}
	}
	else if (method == 0)
	{
		upload = 0;
		status++;
	}
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

void Connection::connectionLog (std::ofstream &accessLog, int flag)
{
	accessLog << std::left << std::setw (20) << currentDateTime () << " " ;
	if (flag == REQUEST)
		accessLog << std::left << std::setw (15) << std::setw (10) << "\e[0;33mREQUEST \e[0m"<< " ";
	else
		accessLog << std::left << std::setw (15) << "\e[0;33mRESPONSE\e[0m"<< " ";
	accessLog << std::left << std::setw (10) << "Peer Address : \e[0;36m" << ipAddress << " \e[0m";
	accessLog << std::left << std::setw (10) << "Connection Port : \e[0;32m" << port << " \e[0m";
	if (flag == REQUEST)
	{
		accessLog << std::left << std::setw (20) <<  "Data recievd :"  << requestLength ;
	}
		
	else
	{
		accessLog << std::left << std::setw (15) << "Data sent :" ;
		accessLog << std::setw (15) << bytesSent;
	}
	if (status == 2)
	{
		accessLog << std::left << std::setw (17) << "Endpoint :" ;
		accessLog << std::setw (10) <<  request->getLocation ()->getEndPoint ();
		accessLog << std::left << std::setw (10) << "Request method: " ;
		accessLog << std::setw (10) << request->getMethod ();
		accessLog  << std::endl;
	}
}

void Connection::connectionLog (std::ofstream &errorLog, std::string error, std::string errorContext)
{
	errorLog << std::left << std::setw (20) << currentDateTime () << " " ;
	errorLog << std::left << std::setw (18) << "\e[0;36m Peer Address : \e[0m";
	errorLog << std::setw(15) << ipAddress << " ";
	errorLog << std::left << std::setw (20) << "\e[0;32m Connection Port : \e[0m";
	errorLog <<  std::setw (10) << port << " ";
	errorLog << std::left << std::setw (10) << "\e[0;31m Error Context: \e[0m" ;
	errorLog << std::setw (20) << errorContext << " ";
	errorLog << std::left << std::setw (20) << "\e[0;31m Error Context: \e[0m" ;
	errorLog << std::setw (20) << error << " ";
	errorLog << std::endl;
}

size_t	timeInMilliseconds(void)
{
	struct timeval	time_stamp;

	gettimeofday (&time_stamp, NULL);
	return ((time_stamp.tv_sec * 1000) + (time_stamp.tv_usec / 1000));
}


void HttpApplication::terminateConnection  (SOCKET fd, std::string addr, int port)
{
	closeConnection (fd);
	std::cout << std::left << std::setw (20) << currentDateTime () << " " ;
	std::cout << std::left << std::setw (18) << "\e[0;36m Peer Address : \e[0m";
	std::cout << std::setw(15) << addr << " ";
	std::cout << std::left << std::setw (20) << "\e[0;32m Connection Port : \e[0m";
	std::cout <<  std::setw (10) << port << "\e[0;31m Connection timeout \e[0m" << std::endl;
	while (1);
}