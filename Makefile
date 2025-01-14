
# FEEL FREE TO EDIT

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
NAME = webserv
FILES = main


SRCS = $(addsuffix .cpp, $(FILES))
OBJS = $(addprefix objs/, $(addsuffix .o, $(FILES)))

# Additional include paths (not used in this Makefile)
# HEADERS =
#INCLUDE_PATHS = -I.

objs/%.o: %.cpp
		@if ! [ -d objs ]; then\
			mkdir objs;\
		fi
		@$(CXX) $(CXXFLAGS) -c -o $@ $<
		

all: $(NAME)

$(NAME): $(HEADERS) $(OBJS)
# @$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -o $@ $(SRCS)
		@$(CXX) $(CXXFLAGS) -o $@ $(SRCS)
		@echo "\033[0;32m ✅ Compilation done! ✅ \033[0m"

clean:
		@rm -rf objs
		@echo "\033[0;32m🧹🧼All cleaned!🧹🧼\033[0m"

fclean: clean
		@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
