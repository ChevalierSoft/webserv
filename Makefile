# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/04/12 12:55:09 by dait-atm          #+#    #+#              #
#    Updated: 2021/09/03 06:13:52 by dait-atm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= webserv

CC			= clang++
FLAGS		= -I$(INC_DIR) #-std=c++98 #-Wall -Wextra -Werror

INC_DIR		= includes/
SRC_DIR 	= srcs/
SOURCES		= $(wildcard $(SRC_DIR)*.cpp)
OBJ_DIR		= objs/
OBJS		= $(patsubst %.cpp, objs/%.o, $(SRCS_FILES))
DEPENDS		= $(patsubst %.cpp, objs/%.d, $(SRCS_FILES))
SRCS_FILES	= \
main.cpp

all: $(NAME)

$(NAME) : $(create_obj_folder) $(OBJS)
	$(CC) -o $(NAME) $(FLAGS) $^ -o $@

-include $(DEPENDS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp Makefile
	$(CC) $(FLAGS) -MMD -MP -c $< -o $@

$(create_obj_folder):
	@mkdir -p objs

clean:
	rm -f $(OBJS) $(DEPENDS)

fclean : clean
	rm -f $(NAME)

.PHONY : all $(NAME) clean fclean re
