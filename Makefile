SDIR =tcp
CFLAGS = -Wall -Wextra -Werror  -g -fsanitize=address #-std=c++98 
CCP =c++
CC =gcc
EXEC=webserv
CORE_SRCS =main.cpp  serverUtils.cpp ServerInstance.cpp  HttpApplication.cpp httpUtils.cpp 
CONF_SRCS = config.cpp utils.cpp
REQUEST_SRCS = Request.cpp requestTools.cpp requestParser.cpp
RESPONSE_SRCS = Response.cpp
CORE_HEADERS = ServerInstance.hpp 
REQUEST_HEADERS = Request.hpp request.h
RESPONSE_HEADERS = Response.hpp
SRCS =  $(addprefix ./config/, $(CONF_SRCS)) $(addprefix ./core/, $(CORE_SRCS)) $(addprefix ./request/, $(REQUEST_SRCS)) $(addprefix ./response/, $(RESPONSE_SRCS))
OBJS = $(SRCS:.cpp=.o)
CONF_HEADERS = config.hpp utils.hpp
HEADERS = $(addprefix ./core/, $(CORE_HEADERS)) $(addprefix ./config/, $(CONF_HEADERS))  $(addprefix ./request/, $(REQUEST_HEADERS)) $(addprefix ./responsee/, $(RESPONSE_HEADERS)) 

all:$(EXEC)

%.o : %.cpp $(HEADERS)
	$(CCP) $(CFLAGS) -c $< -o $@ 
%.o : %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@ 

$(EXEC):$(OBJS) 
	$(CCP) $(CFLAGS) $(OBJS) -o $(EXEC) 

# Connection:	basic_cn_Connection.o
# 	$(CC) $(CFLAGS) $< -o Connection

hoho: all clean
	clear && ./$(EXEC) ./testing/configs/test.conf

clean:
	rm -rf $(OBJS)
fclean: clean
	rm -rf $(EXEC)
re:fclean all
