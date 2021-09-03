# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/04/12 12:55:09 by dait-atm          #+#    #+#              #
#    Updated: 2021/07/13 03:03:44 by dait-atm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= webserv
FLAGS	= #-std=c++98 #-Wall -Wextra -Werror
SRCS	= $(addprefix srcs/, \
poll.cpp \
);

ifeq ($(OS),Windows_NT)
	CC	= g++
else
	CC	= clang++
endif

all : $(NAME)

$(NAME) :
	$(CC) -o $(NAME) $(FLAGS) $(SRCS)

clean :
	rm -f *.o

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY : all $(NAME) clean fclean re