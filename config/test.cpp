#include <iostream>

class Server 
{
    private:
        std::string name;
        std::string host;
    public:
        Server();
        ~Server();
        Server *getServer();
        void    parseServer();
        std::string getName();
        std::string getHost();
};

Server::Server()
{
    std::cout << "Calling the default constructor" << std::endl;
}

Server::~Server()
{
    std::cout << "Default destructor of server" << std::endl;
}

void    Server::parseServer()
{
    this->host = "localhost";
    this->name = "domain.com";
}

std::string Server::getHost()
{
    return this->host;
}

std::string Server::getName()
{
    return (this->name);
}

Server * Server::getServer()
{
    Server *server = new Server();
    server->parseServer();
    return (server);
}

int main(void)
{
    Server test;

    std::cout << test.getServer()->getName() << std::endl;
    std::cout << test.getServer()->getHost() << std::endl;
    return (0);
}