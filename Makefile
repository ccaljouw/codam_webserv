# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: ccaljouw <ccaljouw@student.42.fr>            +#+                      #
#                                                    +#+                       #
#    Created: 2023/06/21 08:27:49 by wingessorac   #+#    #+#                  #
#    Updated: 2023/11/07 12:25:59 by carlo         ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

include common.mk

#==============================================================================#

.DEFAULT_GOAL	:= all

TARGET	 	=	webserv
OBJ_FILES	=	$(addprefix obj/, main.o \
				$(addprefix parsing/, Uri.o HttpRequest.o HttpResponse.o helpers.o) \
				$(addprefix polling/, handlers.o registerEvents.o) \
				$(addprefix config/, Server.o) \
				$(addprefix CGI/, CGI_Handler.o))


#===============================================================================#

all: $(TARGET)

#=========Linking===============================================================#
$(TARGET): $(OBJ_FILES)
	@echo -e "$(GREEN)Linking $(TARGET)$(RESET)"
	@$(CXX) $(LDLAGS) -o $@ $^


#=======Compiling===============================================================#
$(OBJ_FILES): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo -e "$(GREEN)Compiling $@ $(RESET) $(notdir $<)"
	@$(CXX) $(CFLAGS) $(HEADERS) -c -o $@ $<


#=======Cleaning================================================================#
clean:
	@echo -e "$(BLUE)Remoning OBJ files$(RESET)"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo -e "$(BLUE)Removing $(TARGET)$(RESET)"
	@rm -rf $(TARGET)

re: fclean all
