#include "CGI.hpp"

int main(void)
{

    Config *file = new Config("../testing/configs/webserv.conf");
    char fileRequest[] = {"POST index.php/tv/home?season=5&episode=62 HTTP/1.1\r\nHost: 192.120.18.0:80\r\nConnection: keep-alive\r\naccept: */*\r\naccept-encoding: gzip, deflate, br\r\ncontent-type: multipart/form-data\r\naccept-language: en-US,en;q=0.9\r\nsec-fetch-mode: cors\r\n\r\nusername=havel\npassword=sbardila\njwt=34D3F564we3f16e16F651e32164843216482"};
    Request *request = new Request(fileRequest);
    file->parseConfig();

    stringContainer str = request->getBody();
    printContainer(str);
    // Server *server = file->getHttpContext()->getServers()[3];
    // Location *location = file->getHttpContext()->getServers()[3]->getLocations()[1];
    // CGIHandler CGI(location, server, request);
    // CGI.execute();
    return (0);
}