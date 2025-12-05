NAME		=	ircserv

INC_DIR		=	inc/
SRC_DIR		=	src/
OBJ_DIR		=	obj/

CXX			=	c++ -Wall -Werror -Wextra -std=c++98
RM			=	rm -f

SRCS			=	$(SRC_DIR)main.cpp \
					$(SRC_DIR)server.cpp

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
