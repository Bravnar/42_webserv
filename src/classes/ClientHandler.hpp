#ifndef CLIENTHANDLER_HPP
#define CLIENTHANDLER_HPP

# include <netinet/in.h>
# include <unistd.h>
# include "./../utils/Logger.hpp"
# include "./../utils/Convert.hpp"
# include <poll.h>
# include <fcntl.h>
# include "./HttpRequest.hpp"
# include "./HttpResponse.hpp"
# include "./Runtime.hpp"
# include "./ServerManager.hpp"

class Runtime;

class ClientHandler
{
	private:
		std::ostream& fatal(const std::string&);
		std::ostream& error(const std::string&);
		std::ostream& warning(const std::string&);
		std::ostream& info(const std::string&);
		std::ostream& debug(const std::string&);
		Runtime& runtime_;
		ServerManager& server_;
		const sockaddr_in addr_;
		const socklen_t len_;
		int socket_;

	public:
		// Canonical

		ClientHandler(Runtime&, ServerManager&, int client_socket, sockaddr_in client_addr, socklen_t len);
		ClientHandler(const ClientHandler&);
		ClientHandler& operator=(const ClientHandler&) {return *this;};
		~ClientHandler();
		//Member functions

		void handle();
		int getSocket() const;

};

#endif