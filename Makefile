
# FEEL FREE TO EDIT / JE SAIS QUE CELUI CI EST SIMPLE
# OK DE FAIRE UN TRUC PLUS POTENT

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
NAME = webserv
FILES =	main \
		classes/ClientHandler \
		classes/ServerManager \
		classes/HttpRequest \
		classes/HttpResponse \
		classes/ConfigManager \
		classes/ConfigParser \
		classes/Runtime \
		classes/ServerConfig \
		classes/RouteConfig \
		utils/ParseUtils \
		utils/Logger \
		utils/Convert \


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

up: $(HEADERS) Dockerfile docker-compose.yml $(SRCS)
		@docker compose up --build -d
		@echo "\033[0;32m ✅ WebservSIR and Nginx are up! ✅ \033[0m"

down: $(HEADERS) Dockerfile docker-compose.yml $(SRCS)
		@docker compose down
		@echo "\033[0;32m ✅ WebservSIR and Nginx are down! ✅ \033[0m"

logs: $(HEADERS) Dockerfile docker-compose.yml $(SRCS)
		@docker logs 42_webserv-webserv-1

.PHONY: all clean fclean re debug up down logs
