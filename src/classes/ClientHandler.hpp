#ifndef CLIENTHANDLER_HPP
# define CLIENTHANDLER_HPP

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
# include <fstream>

# define EXC_SOCKET_READ "Error reading from socket"
# define EXC_FILE_READ "Error reading from socket"
# define EXC_FILE_NF(file) file + " not found"
# define EXC_FETCHING_BREFORE_READ "Client fetched before read"

class Runtime;

// Unique temporary data
struct s_clientBuffer {
	std::string *requestBuffer;
	std::string *fileBuffer;
	s_clientBuffer(): requestBuffer(0), fileBuffer(0) {}
};

// Unique client address identifier
struct s_clientAddress {
	sockaddr_in addr;
	socklen_t len;
	char clientIp[INET_ADDRSTRLEN];
	s_clientAddress(sockaddr_in addr_, socklen_t len_): addr(addr_), len(len_) {
		inet_ntop(AF_INET, &(addr.sin_addr), clientIp, INET_ADDRSTRLEN);
	}
	s_clientAddress() {
		std::memset(&addr, 0, sizeof(sockaddr_in));
		std::memset(&len, 0, sizeof(socklen_t));
		std::memset(&clientIp, 0, INET6_ADDRSTRLEN);
	}
};

// Unique client state
struct s_clientState {
	bool isFetched;
	bool isReading;
	s_clientState(): isFetched(false), isReading(false) {}
};

class ClientHandler
{
	private:
		ClientHandler(const ClientHandler&);
		ClientHandler& operator=(const ClientHandler&);
		
		std::ostream& fatal(const std::string&);
		std::ostream& error(const std::string&);
		std::ostream& warning(const std::string&);
		std::ostream& info(const std::string&);
		std::ostream& debug(const std::string&);
		void fillRequestBuffer_();
		void fillFileBuffer_(std::ifstream& input);

		int socket_fd_; // Unique client socket indentifier
		Runtime& runtime_;
		ServerManager& server_;
		s_clientAddress address_;
		s_clientBuffer buffer_;
		HttpRequest request_;
		HttpResponse response_;
		s_clientState state_;

	public:
		// Canonical

		ClientHandler(Runtime&, ServerManager&, int socket_fd, sockaddr_in addr, socklen_t addrlen);
		~ClientHandler();
		//Member functions

		void handle();
		int getSocket() const;
		const HttpRequest& fetch();
		int readSocket();
		const HttpResponse& getResponse() const;
		const char *getClientIp() const;
		bool isFetched() const;
		bool isReading() const;
		void setReading(bool);
		const ServerManager& getServer() const;
};

#endif