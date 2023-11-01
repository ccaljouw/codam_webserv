# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    common.mk                                          :+:    :+:             #
#                                                      +:+                     #
#    By: cwesseli <cwesseli@student.codam.nl>         +#+                      #
#                                                    +#+                       #
#    Created: 2023/07/19 17:26:03 by cwesseli      #+#    #+#                  #
#    Updated: 2023/11/01 08:35:40 by carlo         ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

#=============Compiler and Flags===============================================#
CXX		 	?=	c++
CFLAGS	 	?=	-Wall -Wextra -Werror -Wshadow 
# -std=c++98

LDFLAGS	 	?=	$(CFLAGS)


#=============Directories and include==========================================#
INCLUDE		?=	./include
HEADERS		?=	-I$(INCLUDE)
SRC_DIR		?=	./src
OBJ_DIR		=	./obj


#=============Options==========================================================#
run:	
	make && ./$(TARGET)


debug:	CFLAGS = -Wall -Wextra -g
debug:	re
	@echo -e "$(YELLOW)\n!-DEBUG MODE-!\n$(RESET)"


val:	CFLAGS = -Wall -Wextra
val:	re
	valgrind --leak-check=full --show-leak-kinds=all -s ./$(TARGET)

#=============Other=============================================================#

.PHONY:	all clean fclean re run debug val


#=============Colors============================================================#

RESET	=	\033[0m
GREEN	=	\033[32;1m
BLUE	=	\033[34;1m
YELLOW	=	\033[0;33m