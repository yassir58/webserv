#include "CGI.hpp"

// std::string createRequest()
// {
// 	std::string Method = "POST";
//     std::string URL = "http://localhost/index.php?season=5&episode=62"; 
// std::string URL = "http://hoho.com/sbardila?sbarila";
//     std::string Headers = "Host: 192.120.18.0:80\r\nConnection: keep-alive\r\naccept: */*\r\naccept-encoding: gzip, deflate, br\r\ncontent-type: application/x-www-form-urlencoded\r\naccept-language: en-US,en;q=0.9\r\nsec-fetch-mode: cors\r\n";
//     std::string Body = "username=havel\npassword=sbardila\njwt=34D3F564we3f16e16F651e32164843216482";

//     std::stringstream request;
//     request << Method << " " << URL << " HTTP/1.1\r\n";
//     request << Headers;
//     request << "Content-Length: " << Body.length() << "\r\n";
//     request << "\r\n";
//     request << Body;

//     std::string request_str = request.str();
// 	return (request_str);
// }

int main(void)
{
    Config *file = new Config("../testing/configs/webserv.conf");
    char fileRequest[] = {"POST http://localhost/index.py/tv/home?season=5&episode=62 HTTP/1.1\r\nHost: 192.120.18.0:80\r\nConnection: keep-alive\r\naccept: */*\r\naccept-encoding: gzip, deflate, br\r\ncontent-type: application/x-www-form-urlencoded\r\naccept-language: en-US,en;q=0.9\r\nsec-fetch-mode: cors\r\nCookie: user_status=1; user_uid=22c867a3;"};
    Request *request = new Request(fileRequest);
    file->parseConfig();

    // stringContainer str = request->getBody();
    // printContainer(str);
    Server *server = file->getHttpContext()->getServers()[3];
    Location *location = file->getHttpContext()->getServers()[3]->getLocations()[1];
    CGIHandler CGI(location, server, request);
    CGI.execute();
    return (0);
}