#ifndef CLIENTHANDLER_HPP
#define CLIENTHANDLER_HPP

# include <netinet/in.h>
# include <unistd.h>
# include <arpa/inet.h>
# include "./../utils/Logger.hpp"

class ClientHandler
{
	private:
		const int client_fd_;
		const sockaddr_in client_addr_;
		const socklen_t client_len_;

	public:
		ClientHandler(int client_socket, sockaddr_in client_addr, socklen_t len);
		ClientHandler(const ClientHandler&);
		ClientHandler& operator=(const ClientHandler&) {return *this;};
		~ClientHandler();
		int handle();
};

#endif