
# FEEL FREE TO EDIT / JE SAIS QUE CELUI CI EST SIMPLE
# OK DE FAIRE UN TRUC PLUS POTENT

CXX = c++
GXX = /opt/homebrew/opt/gcc@14/bin/g++-14

CXXFLAGS = -Wall -Werror -Wextra -std=c++98
NAME = webserv
FILES =	main \
		classes/ClientHandler \
		classes/ServerManager \
		classes/HttpRequest \
		classes/HttpResponse \
		classes/ListingBuilder \
		classes/ConfigManager \
		classes/ErrorBuilder \
		classes/ConfigParser \
		classes/Runtime \
		classes/ServerConfig \
		classes/RouteConfig \
		classes/CgiHandler \
		utils/ParseUtils \
		utils/ReplaceAll \
		utils/Logger \
		utils/Convert \


SRCS = $(addsuffix .cpp, $(addprefix src/, $(FILES)))
OBJS = $(addprefix objs/, $(addsuffix .o, $(FILES)))

# Additional include paths (not used in this make -sfile)
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
		@$(CXX) $(CXXFLAGS) -OFast -funroll-loops -finline-functions -o $(NAME) $(OBJS)
		@echo "\033[0;32m ✅ Compilation done! ✅ \033[0m"

gnu: $(HEADERS) $(SRCS) $(OBJS)
# @$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -o $@ $(SRCS)
		@$(GXX) $(CXXFLAGS) -OFast -funroll-loops -finline-functions -o $(NAME) $(OBJS)
		@echo "\033[0;32m ✅ Compilation done! ✅ \033[0m"

clean:
		@rm -rf objs
		@echo "\033[0;32m🧹🧼All cleaned!🧹🧼\033[0m"

fclean: clean
		@rm -rf $(NAME)
		@rm -rf ./www/siege/randomfile
		@rm -rf $(NAME).dSYM
		@rm -rf vgcore.*

re:
	@make -s fclean
	@make -s all

debug : $(HEADERS) $(SRCS) $(OBJS)
	@if [ "$(shell uname)" = "Linux" ]; then\
		$(CXX) $(CXXFLAGS) -o $(NAME) $(SRCS) -g3 -D LOGGER_DEBUG=1; \
		echo " \t$(NAME) compiled with debug for Linux ✅"; \
	elif [ "$(shell uname)" = "Darwin" ]; then \
		$(CXX) $(CXXFLAGS) -o $(NAME) $(SRCS) -g3 -D LOGGER_DEBUG=1 -fsanitize=address; \
		echo " \t$(NAME) compiled with debug for MacOS ✅"; \
	fi

up: $(HEADERS) Dockerfile docker-compose.yml $(SRCS)
	@if [ "$(shell uname)" = "Linux" ]; then\
		docker compose up --build -d;\
		echo "\033[0;32m ✅ WebservSIR and Nginx are up for Linux! ✅ \033[0m";\
	elif [ "$(shell uname)" = "Darwin" ]; then \
		docker-compose up --build -d; \
		echo "\033[0;32m ✅ WebservSIR and Nginx are up for MacOS! ✅ \033[0m";\
	fi

down: $(HEADERS) Dockerfile docker-compose.yml $(SRCS)
		@docker compose down
		@echo "\033[0;32m ✅ WebservSIR and Nginx are down! ✅ \033[0m"

logs: Dockerfile docker-compose.yml $(SRCS)
		@docker logs 42_webserv-webserv-1

randomfile:
	@dd if=/dev/urandom of=./www/siege/randomfile bs=1M count=10240

.PHONY: all clean fclean re debug up down logs randomfile
