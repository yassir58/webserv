SDIR=tcp
CFLAGS=-Wall -Wextra -Werror
CCP=c++
CC=gcc
SRCS= basic_cn_server.cpp basic_cn_client.cpp
OBJS= $(SRCS:.c=.o)
HEADERS=basic_cn_server.hpp 

all:server client

%.o : %.cpp $(HEADERS)
	$(CCP) $(CFLAGS) -c $< -o $@ 
%.o : %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@ 

server:	basic_cn_server.o
	$(CCP) $(CFLAGS) -o server basic_cn_server.o

client:	basic_cn_client.o
	$(CC) $(CFLAGS) -o client basic_cn_client.o
