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
# include "./HttpResponse.hpp"

# define EXC_SOCKET_READ "Error reading from socket"
# define EXC_FILE_READ "Error reading from file"
# define EXC_FETCHING_BREFORE_READ "Client fetched before read"

class Runtime;

// Unique temporary data
struct s_clientBuffer {
	std::string *requestBuffer;
	std::ifstream *fileStream;
	s_clientBuffer():
		requestBuffer(0),
		fileStream(0) {}
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
	bool isSent;
	bool isSending;
	bool isDead;
	bool hasResponse;
	s_clientState():
		isFetched(false),
		isReading(false),
		isSent(false),
		isSending(false),
		isDead(false),
		hasResponse(false) {}
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
		void sendHeader_();
		void sendPlayload_();
		std::string buildDirlist_();

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

		void sendResponse();
		int getSocket() const;
		void readSocket();
		const HttpRequest& buildRequest();
		const HttpRequest& getRequest() const;
		const HttpResponse& buildResponse(const HttpResponse& response);
		const HttpResponse& getResponse() const;
		const char *getClientIp() const;
		bool isFetched() const;
		void setFetched(bool);
		bool isReading() const;
		void setReading(bool);
		bool isSending() const;
		bool isSent() const;
		bool isDead() const;
		bool hasResponse() const;
		const ServerManager& getServer() const;
		std::ifstream *getFileStream();
};

#endif