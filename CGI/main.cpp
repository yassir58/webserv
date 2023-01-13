#include "CGI.hpp"

std::string toUpperCase(std::string str)
{
    int i;

    i = 0;
    while (str[i])
    {
        str[i] = toupper(str[i]);
        i++;
    }
    return (str);
}

int main(void)
{
    char fileRequest[] = {"GET http//127.1.1.1:00/rfc/rfc2616/tv/home?season=5&episode=62 HTTP/1.1\r\nHost: 192.120.18.0:80\r\nConnection: keep-alive\r\naccept: */*\r\naccept-encoding: gzip, deflate, br\r\naccept-language: en-US,en;q=0.9\r\nsec-fetch-mode: cors\r\n\r\nsomething"};
    Request newRequest(fileRequest);
    
    headerFieldList value = newRequest.getHeaderField();

   	headerFieldList::iterator end = value.end();
	headerFieldList::iterator begin = value.begin();

	for (; begin != end; ++begin)
	{
		std::cout << "HTTP_" << toUpperCase(begin->key) << "=" << begin->value << std::endl;
	}
    return (0);
}