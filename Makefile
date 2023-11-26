# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: bfranco <bfranco@student.codam.nl>           +#+                      #
#                                                    +#+                       #
#    Created: 2023/06/21 08:27:49 by wingessorac   #+#    #+#                  #
#    Updated: 2023/11/26 18:27:02 by bfranco       ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

include common.mk

#==============================================================================#

.DEFAULT_GOAL	:= all

TARGET	 	=	webserv
OBJ_FILES	=	$(addprefix obj/, main.o utils.o \
				$(addprefix parsing/, Uri.o HttpRequest.o HttpResponse.o) \
				$(addprefix polling/, handlers.o registerEvents.o) \
				$(addprefix config/, Server.o Config.o ConfigParserUtils.o) \
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
	@rm -rf uploads
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo -e "$(BLUE)Removing $(TARGET)$(RESET)"
	@rm -rf $(TARGET)

re: fclean all
