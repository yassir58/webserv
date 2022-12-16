// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#define PORT 8080
#include "pthread.h"




void *thread_executor (void *test)
{
	int sock = 0, valread, client_fd;
	struct sockaddr_in serv_addr;
	char* hello = "Hello from client";
	char buffer[1024] = { 0 };
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return NULL;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
		<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
		return NULL;
	}

	if ((client_fd
		= connect(sock, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr)))
		< 0) {
		printf("\nConnection Failed \n");
		return NULL;
	}
	send(sock, hello, strlen(hello), 0);
	printf("Hello message sent\n");
	sleep (20);
	send (sock, hello, strlen (hello), 0);
	valread = read(sock, buffer, 1024);
	printf("%s\n", buffer);

	(void)valread;
	(void)test;

	// closing the connected socket
	close (sock);
	close(client_fd);
	return (NULL);
}


void *myThreadFun(void *vargp)
{
	char *arg;

	arg = (char *)vargp;
	if (!strcmp("local", arg))
		system ("curl http://localhost:8080");
	else
		system ("curl http://192.168.1.140:8080");
    return NULL;
}
  
int main(int argc, char *argv[])
{
    pthread_t thread_id[100];

	for (int i = 0; i < 100; i++)
	{
		pthread_create(&thread_id[i], NULL, myThreadFun, argv[argc - 1]);
	}
	for (int i = 0; i < 100; i++)
		pthread_join(thread_id[i], NULL);
	return (0);
}