SDIR=tcp
CFLAGS=-Wall -Wextra -Werror
CCP=c++
CC=gcc
SRCS= main.cpp  server_utils.cpp Server_instance.cpp
OBJS= $(SRCS:.cpp=.o)
HEADERS=basic_cn_server.hpp 

all:server

%.o : %.cpp $(HEADERS)
	$(CCP) $(CFLAGS) -c $< -o $@ 
%.o : %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@ 

server:$(OBJS)
	$(CCP) $(CFLAGS) $(OBJS) -o server 

# client:	basic_cn_client.o
# 	$(CC) $(CFLAGS) $< -o client


clean:
	rm -rf $(OBJS)
fclean: clean
	rm -rf server
re:fclean all