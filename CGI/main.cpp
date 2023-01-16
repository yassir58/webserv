#include "CGI.hpp"

int main(void)
{

    Config *file = new Config("../config/webserv.conf");
    char fileRequest[] = {"GET http//127.1.1.1:00/rfc/rfc2616/index.php/tv/home?season=5&episode=62 HTTP/1.1\r\nHost: 192.120.18.0:80\r\nConnection: keep-alive\r\naccept: */*\r\naccept-encoding: gzip, deflate, br\r\naccept-language: en-US,en;q=0.9\r\nsec-fetch-mode: cors\r\n\r\nsomething"};
    Request *request = new Request(fileRequest);
    
    file->parseConfig();

    Location *location = file->getHttpContext()->getServers()[3]->getLocations()[1];
    Server *server = file->getHttpContext()->getServers()[3];
    CGIHandler CGI(location, server, request);
    CGI.execute();
    return (0);
}