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
		// Forbidden Canonical
		// (ClientHandler is unique and interacts with runtime at construction)

		ClientHandler(const ClientHandler&);
		ClientHandler& operator=(const ClientHandler&);
	
		// Internal logs

		std::ostream& fatal(const std::string&);
		std::ostream& error(const std::string&);
		std::ostream& warning(const std::string&);
		std::ostream& info(const std::string&);
		std::ostream& debug(const std::string&);

		// Internal functions
	
		// Fill client request buffer
		// @throw `EXC_SOCKET_READ`
		void fillRequestBuffer_();
		// Send response header to client
		// @throw `EXC_SEND_ERROR`
		void sendHeader_();
		// Send playload to client
		// @throw `EXC_SEND_ERROR`
		void sendPlayload_();
		// TODO: Replace with proper Builder class
		// Build a directory list page
		std::string buildDirlist_();


		// Properties

		const int socket_fd_;
		Runtime& runtime_;
		const ServerManager& server_;
		const s_clientAddress address_;
		s_clientBuffer buffer_;
		HttpRequest request_;
		HttpResponse response_;
		s_clientState state_;

	public:
		// Canonical

		ClientHandler(Runtime&, ServerManager&, int socket_fd, sockaddr_in addr, socklen_t addrlen);
		~ClientHandler();

		//Member functions

		// Send full response to client (header and chunks of playload)
		// @throw `EXC_SEND_ERROR`
		void sendResponse();
		// Get client socket fd
		int getSocket() const;
		// Read socket and fill requestBuffer by chunks until no more to read
		// @throw `EXC_SOCKET_READ`
		void readSocket();
		// Build request from request buffer
		// Returns the request if already fetched
		// @throw `HttpRequest(const std::string*)` constructor exceptions
		const HttpRequest& buildRequest();
		// Build client response from `const HttpResponse&` template
		const HttpResponse& buildResponse(const HttpResponse&);
		// Get current response

		// Getters
		const HttpRequest& getRequest() const;
		const HttpResponse& getResponse() const;
		const char *getClientIp() const;
		bool isFetched() const;
		bool isReading() const;
		bool isSending() const;
		bool isSent() const;
		bool isDead() const;
		bool hasResponse() const;
		const ServerManager& getServer() const;
		std::ifstream *getFileStream();

		// Setters
	
		void setFetched(bool);
		void setReading(bool);
};

#endif