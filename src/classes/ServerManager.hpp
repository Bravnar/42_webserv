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

class ServerManager {
	private:
		const int port_;
		const struct sockaddr_in addrv4_;
		const struct sockaddr *address_;
		bool isHealthy_;
		int server_fd_;
		int init_();

	public:
		ServerManager();
		ServerManager(int);
		ServerManager(const ServerManager&);
		ServerManager& operator=(const ServerManager&);
		~ServerManager();
		bool isHealthy();
		int run();
};

#endif