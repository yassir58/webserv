SDIR =tcp
CFLAGS =-Wall -Wextra -Werror  -g #-std=c++98
CCP =c++
CC =gcc
CORE_SRCS =main.cpp  serverUtils.cpp ServerInstance.cpp  HttpApplication.cpp httpUtils.cpp 
CONF_SRCS = config.cpp utils.cpp
REQUEST_SRCS = Request.cpp
RESPONSE_SRCS = Response.cpp
CORE_HEADERS = ServerInstance.hpp 
REQUEST_HEADERS = Request.hpp request.h
SRCS =  $(addprefix ./config/, $(CONF_SRCS)) $(addprefix ./core/, $(CORE_SRCS)) $(addprefix ./request/, $(REQUEST_SRCS)) 
OBJS = $(SRCS:.cpp=.o)
CONF_HEADERS = config.hpp utils.hpp
HEADERS = $(addprefix ./core/, $(CORE_HEADERS)) $(addprefix ./config/, $(CONF_HEADERS))  $(addprefix ./request/, $(REQUEST_HEADERS)) 

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
