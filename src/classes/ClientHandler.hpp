#ifndef CLIENTHANDLER_HPP
#define CLIENTHANDLER_HPP

# include <netinet/in.h>
# include <unistd.h>
# include <arpa/inet.h>
# include "./../utils/Logger.hpp"
# include "./ServerManager.hpp"
# include "./../utils/Convert.hpp"
# include <poll.h>

class ServerManager;

class ClientHandler
{
	private:
		ServerManager& server_;
		const sockaddr_in addr_;
		const socklen_t len_;
		pollfd *socket_;

	public:
		ClientHandler(ServerManager&, int client_socket, sockaddr_in client_addr, socklen_t len);
		ClientHandler(const ClientHandler&);
		ClientHandler& operator=(const ClientHandler&) {return *this;};
		~ClientHandler();
		int handle();
		const pollfd& getSocket() const;

};

#endif