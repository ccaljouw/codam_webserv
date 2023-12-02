# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: ccaljouw <ccaljouw@student.42.fr>            +#+                      #
#                                                    +#+                       #
#    Created: 2023/06/21 08:27:49 by wingessorac   #+#    #+#                  #
#    Updated: 2023/12/02 22:18:50 by cariencaljo   ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

include common.mk

#==============================================================================#

.DEFAULT_GOAL	:= all

TARGET	 	=	webserv
OBJ_FILES	=	$(addprefix obj/, main.o  \
				$(addprefix parsing/, Uri.o HttpRequest.o HttpResponse.o) \
				$(addprefix polling/, connectionHandling.o requestHandling.o CGIHandler.o registerEvents.o) \
				$(addprefix utils/, utils.o errorPageGen.o cookieGen.o) \
				$(addprefix config/, Server.o Config.o ConfigParserUtils.o) \
				$(addprefix CGI/, CGI.o))


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
	@rm -rf tmp
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo -e "$(BLUE)Removing $(TARGET)$(RESET)"
	@rm -rf $(TARGET)

re: fclean all
