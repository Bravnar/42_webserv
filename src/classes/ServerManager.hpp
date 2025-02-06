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

# define DF_PORT 8080
# define DF_INTERFACE "0.0.0.0"

class ClientHandler;

typedef struct s_status {
	bool isHealthy;
	bool isRunning;
} t_status;

typedef struct s_config {
	std::string name;
	std::string interface;
	int port;
	int max_clients;
} t_config;

class ServerManager {
	private:
		int init_();
		const struct sockaddr_in addrv4_;
		const struct sockaddr *address_;
		t_status status_;
		t_config config_;
		int server_fd_;
		std::vector<pollfd> sockets_;
		std::vector<ClientHandler *> clients_;

	public:
		// Canonical
		ServerManager();
		ServerManager(int);
		ServerManager(const ServerManager&);
		ServerManager& operator=(const ServerManager&);
		~ServerManager();
		// Member functions
		bool isHealthy() const;
		int runServer();
		void closeServer();
		std::vector<pollfd>& getSockets();
		std::vector<ClientHandler *>& getClients();
};

#endif