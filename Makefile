# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/04/12 12:55:09 by dait-atm          #+#    #+#              #
#    Updated: 2022/03/03 15:57:55 by dait-atm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= webserv

CC			= clang++
FLAGS		= -I$(INC_DIR) -std=c++98 -pthread -Wall -Wextra -Werror

SRC_DIR 	= srcs/
OBJ_DIR		= objs/
INC_DIR		= includes/
SOURCES		= $(wildcard $(SRC_DIR)*.cpp)
OBJS		= $(patsubst %.cpp,objs/%.o, $(SRCS_FILES))
DEPENDS		= $(patsubst %.cpp,objs/%.d, $(SRCS_FILES))
SRCS_FILES	= \
Client.cpp \
Conf.cpp \
directory_listing.cpp \
main.cpp \
Parser.cpp \
ResponseGenerator.cpp \
Request.cpp \
Route.cpp \
Server.cpp \
set_content_types.cpp \
threads.cpp \
utils.cpp

all: $(NAME)

$(NAME) : $(OBJS)
	$(CC) -o $(NAME) $(FLAGS) $^ -o $@

-include $(DEPENDS)

$(OBJ_DIR)%.o : $(create_obj_folder) $(SRC_DIR)%.cpp Makefile
	@mkdir -p objs
	$(CC) $(FLAGS) -MMD -MP -c $< -o $@

$(create_obj_folder) :
	mkdir -p objs

clean :
	rm -f $(OBJS) $(DEPENDS)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY : all $(NAME) clean fclean re
