#include "basic_cn_server.hpp"




int main (int argc , char *argv[])
{
    (void)argc;
    (void)argv;

    int sk_fd, connection_sock;
    struct sockaddr_in test_addr; 
    int addr_len = sizeof(test_addr);
    char buffer[1028];
    int read_rt;


    test_addr.sin_family = AF_INET; 
    test_addr.sin_addr.s_addr = INADDR_ANY; 
    test_addr.sin_port = htons( PORT ); 

    sk_fd = socket (AF_INET, SOCK_STREAM, 0);
    if (bind (sk_fd, (struct sockaddr*) &test_addr, sizeof (test_addr)) == -1)
        std::cerr << "\e[0;31m BIND ERROR \e[0m" << std::endl;
    if (listen (sk_fd, 100) == -1)
        std::cerr << "\e[0;31m LISTEN ERROR \e[0m" << std::endl;
    std::cout << "server address : " << test_addr.sin_addr.s_addr << std::endl;
    std::cout << "server listen on port " << test_addr.sin_port << std::endl;
    while (1)
    {
        if ((connection_sock = accept(sk_fd, (sockaddr *)&test_addr, (socklen_t*)&addr_len)) == -1)
            std::cerr << "\e[0;31m ACCEPT ERROR \e[0m" << std::endl;
        read_rt = read(connection_sock, buffer, 1024);
        if (read_rt == 0)
            std::cerr << "\e[0;31m NOTHING TO READ \e[0m" << std::endl;
        std::cout << "message sent by client :" << std::endl;
        std::cout << buffer << std::endl;
        send (connection_sock, buffer, strlen (buffer), 0);
    }

    return (0);
}