#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include <netinet/in.h>
# include <cstring>
# include <unistd.h>
# include <iostream>
# include <cerrno>
# include "./../utils/Logger.hpp"
# include "./../utils/Convert.hpp"
# include <poll.h>
# include <vector>
# include "./ConfigManager.hpp"
# include <arpa/inet.h>

# define DF_PORT 8080
# define DF_INTERFACE "0.0.0.0"
# define DF_MAX_BUFFER 8192

typedef struct s_status {
	bool isHealthy;
	bool isRunning;
} t_status;

class ServerManager {
	private:
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
		pollfd socket_;

	public:
		// Canonical

		ServerManager(const ServerConfig&);
		ServerManager(const ServerManager&);
		ServerManager& operator=(const ServerManager&);
		~ServerManager();
		// Member functions

		int init();
		bool isHealthy() const;
		const pollfd& getSocket() const;
		const ServerConfig& getConfig() const;
		const std::vector<RouteConfig>& getRouteConfig() const;
};

#endif