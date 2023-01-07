# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/01/06 16:09:42 by Ma3ert            #+#    #+#              #
#    Updated: 2023/01/06 17:46:18 by Ma3ert           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC = main.cpp ./response/Response.cpp ./request/Request.cpp 

OBJ = $(SRC:.cpp=.o)

NAME = test

HEADER = ./response/Response.hpp ./request/Request.hpp ./request/Request.h

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

%.o : %.cpp $(HEADER)
	$(CC) $(FLAGS) -c $< -o $@

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

hoho: all
	clear && ./$(NAME) && make fclean

sbrdl: all clean
	clear

re: fclean all