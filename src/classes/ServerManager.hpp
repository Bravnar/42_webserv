#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include <netinet/in.h>
# include <cstring>
# include <unistd.h>
# include <iostream>
# include <cerrno>
# include "./../utils/Logger.hpp"
# include "./../utils/Convert.hpp"
# include "./ClientHandler.hpp"
# include <poll.h>
# include <vector>
# include "./ConfigManager.hpp"

# define DF_PORT 8080
# define DF_INTERFACE "0.0.0.0"
# define DF_MAX_BUFFER 524288

class ClientHandler;

typedef struct s_status {
	bool isHealthy;
	bool isRunning;
} t_status;

class ServerManager {
	private:
		int init_();
		std::ostream& fatal(const std::string&);
		std::ostream& error(const std::string&);
		std::ostream& warning(const std::string&);
		std::ostream& info(const std::string&);
		std::ostream& debug(const std::string&);
		const ServerConfig& config_;
		const struct sockaddr_in addrv4_;
		const struct sockaddr *address_;
		const std::vector<RouteConfig>& routeconfig_;
		t_status status_;
		int server_fd_;
		std::vector<pollfd> sockets_;
		std::vector<ClientHandler *> clients_;

	public:
		// Canonical

		ServerManager(const ServerConfig&);
		ServerManager(const ServerManager&);
		ServerManager& operator=(const ServerManager&);
		~ServerManager();
		// Member functions

		bool isHealthy() const;
		int runServer();
		void closeServer();
		std::vector<pollfd>& getSockets();
		std::vector<ClientHandler *>& getClients();
		const ServerConfig& getConfig() const;
		const std::vector<RouteConfig>& getRouteConfig() const;
};

#endif