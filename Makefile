
# FEEL FREE TO EDIT / JE SAIS QUE CELUI CI EST SIMPLE
# OK DE FAIRE UN TRUC PLUS POTENT

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
NAME = webserv
FILES =	main \
		utils/Logger \


SRCS = $(addsuffix .cpp, $(addprefix src/, $(FILES)))
OBJS = $(addprefix objs/, $(addsuffix .o, $(FILES)))

# Additional include paths (not used in this Makefile)
# HEADERS =
#INCLUDE_PATHS = -I.

objs/%.o: src/%.cpp
		@mkdir -p $(dir $@)
		@if ! [ -d objs ]; then\
			mkdir objs;\
		fi
		@$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(NAME)

$(NAME): $(HEADERS) $(SRCS) $(OBJS)
# @$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -o $@ $(SRCS)
		@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
		@echo "\033[0;32m ✅ Compilation done! ✅ \033[0m"

clean:
		@rm -rf objs
		@echo "\033[0;32m🧹🧼All cleaned!🧹🧼\033[0m"

fclean: clean
		@rm -rf $(NAME)

re: fclean all

debug:  $(HEADERS) $(SRCS) $(OBJS)
		@$(CXX) $(CXXFLAGS) -o $(NAME) $(SRCS) -g -D LOGGER_DEBUG=1
		@echo "\033[0;32m ✅ Compilation done (debug mode)! ✅ \033[0m"

.PHONY: all clean fclean re debug
