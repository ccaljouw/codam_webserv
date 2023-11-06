# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: ccaljouw <ccaljouw@student.codam.nl>         +#+                      #
#                                                    +#+                       #
#    Created: 2023/08/30 10:48:44 by ccaljouw      #+#    #+#                  #
#    Updated: 2023/11/04 00:25:55 by cariencaljo   ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

#//= Colors =//#
BOLD	:= \033[1m
GREEN	:= \033[32;1m
BLUE	:= \033[34;1m
RESET	:= \033[0m

#//= Variables = //#
NAME		= webserv
CC			= c++
CFLAGS		= -Wall -Wextra -Werror -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wzero-as-null-pointer-constant \
				-Wunused -Woverloaded-virtual -Wconversion -Wsign-conversion -Wfloat-conversion -Wformat=2 -Werror=vla \
				-Wnull-dereference #-std=c++98
INCLUDE		= -I include/ -I classes/
CLASS_FILES = $(addprefix obj/, Server.o Client.o)
OBJ_FILES	= $(addprefix obj/, main.o handlers.o registerEvents.o)

all: $(NAME)
	
$(NAME): $(CLASS_FILES) $(OBJ_FILES) 
	@$(CC)  $(CLASS_FILES) $(OBJ_FILES) -o $(NAME) $(CFLAGS) $(INCLUDE)

$(CLASS_FILES): obj/%.o: classes/%.cpp 
	@mkdir -p $(dir $@)
	@echo "$(GREEN)$(BOLD)Compiling $(NAME):$(RESET) $(notdir $<)"
	@$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<
	
$(OBJ_FILES): obj/%.o: src/%.cpp 
	@mkdir -p $(dir $@)
	@echo "$(GREEN)$(BOLD)Compiling $(NAME):$(RESET) $(notdir $<)"
	@$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

clean:
	@echo "$(BLUE)Cleaning $(NAME) $(RESET)"
	@rm -rf obj/
	@rm -rf obj_builtin/

fclean: clean
	@rm -rf $(NAME)

debug: CFLAGS = -Wall -Wextra 
debug: all

re: fclean all

.PHONY:	all bonus clean fclean re debug