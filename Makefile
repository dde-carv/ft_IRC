NAME		=	ircserv

INC_DIR		=	inc/
SRC_DIR		=	src/
CMD_DIR		=	src/cmd/
OBJ_DIR		=	obj/

CXX			=	c++ -Wall -Werror -std=c++98
RM			=	rm -f

SRCS			=	$(SRC_DIR)main.cpp \
					$(SRC_DIR)Server.cpp \
					$(SRC_DIR)Client.cpp \
					$(SRC_DIR)Channel.cpp \
					$(SRC_DIR)aux.cpp \
					$(CMD_DIR)invite.cpp \
					$(CMD_DIR)join.cpp \
					$(CMD_DIR)kick.cpp \
					$(CMD_DIR)mode.cpp \
					$(CMD_DIR)nick.cpp \
					$(CMD_DIR)part.cpp \
					$(CMD_DIR)pass.cpp \
					$(CMD_DIR)privmsg.cpp \
					$(CMD_DIR)quit.cpp \
					$(CMD_DIR)topic.cpp \
					$(CMD_DIR)user.cpp


OBJS			=	$(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRCS))

all:			$(NAME)

$(NAME):		$(OBJS)
				@$(CXX) $(OBJS) -o $(NAME)
				@echo "\033[1;32m$(NAME) created.\033[0m"

$(OBJ_DIR)%.o:	$(SRC_DIR)%.cpp
				@mkdir -p $(@D)
				@$(CXX) -I$(INC_DIR) -c $< -o $@

clean:
			@$(RM) -r $(OBJ_DIR)
			@echo "\033[1;31mObject files deleted.\033[0m"

fclean:		clean
			@$(RM) $(NAME)
			@echo "\033[1;31m$(NAME) deleted.\033[0m"

re:			fclean all

.PHONY:		all clean fclean re
