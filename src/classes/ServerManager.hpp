#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include <netinet/in.h>
# include <cstring>
# include <unistd.h>

class ServerManager {
	private:
		int port_;
		int server_fd_;
		struct sockaddr_in addrv4_;
		struct sockaddr *address_;
		void init_();

	public:
		ServerManager();
		ServerManager(int port);
		ServerManager(const ServerManager&);
		ServerManager& operator=(const ServerManager&);
		~ServerManager();
};

#endif