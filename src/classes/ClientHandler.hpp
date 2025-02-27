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
# include "./CgiHandler.hpp"
# include <fstream>
# include "./HttpResponse.hpp"
# include <sys/stat.h>
# include "./ListingBuilder.hpp"
# include "../utils/ft_strnstr.hpp"

# define EXC_SOCKET_READ "Error reading from socket"
# define EXC_FILE_READ "Error reading from file"
# define EXC_NO_ROUTE "No valid route found"
# define EXC_BODY_TOO_LARGE "The request body is too large"

// Flags for state

# define FETCHED 0x01 // Has a request
# define READING 0x02 // Still reading request
# define SENT 0x04 // Response sent
# define SENDING 0x08 // Still sending response
# define RESPONSE 0x10 // Has a response


class Runtime;

// Unique temporary data
struct s_clientBuffer {
	std::string *requestBuffer;
	std::string bodyBuffer;
	std::string internalBody;
	std::string boundary;
	std::ifstream externalBody;
	bool bodyReading;
	s_clientBuffer():
		requestBuffer(0),
		bodyBuffer(""),
		internalBody(),
		externalBody(),
		bodyReading(false){}
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
	
		// Send response header to client
		// @throw `EXC_SEND_ERROR`
		void sendHeader_();
		// Send payload to client
		// @throw `EXC_SEND_ERROR`
		void sendpayload_();

		// Properties

		const int socket_fd_;
		Runtime& runtime_;
		const ServerManager& server_;
		const s_clientAddress address_;
		s_clientBuffer buffer_;
		HttpRequest request_;
		HttpResponse response_;
		int8_t flags_;
		unsigned long long last_alive_;

	public:
		// Canonical

		ClientHandler(Runtime&, ServerManager&, int socket_fd, sockaddr_in addr, socklen_t addrlen);
		~ClientHandler();

		//Member functions
		int parseBodyInfo(std::string *request, bool bodyLen);
		// Send full response to client (header and chunks of payload)
		// @throw `EXC_SEND_ERROR`
		// @throw `buildResponse()` throws
		void sendResponse();
		// Read socket and fill requestBuffer by chunks until no more to read
		// @throw `EXC_SOCKET_READ`
		void readSocket();
		// Build request from request buffer
		// Returns the request if already fetched
		// @throw `HttpRequest(const std::string*)` constructor exceptions
		const HttpRequest& buildRequest();
		// Build client response from `HttpResponse` template
		// @throw `EXC_NO_ROUTE`
		const HttpResponse& buildResponse(HttpResponse);
		// Flush client state
		void flush();

		// Getters
		const HttpRequest& getRequest() const;
		HttpResponse& getResponse();
		const char *getClientIp() const;
		int8_t getFlags() const;
		void clearFlag(int8_t flag);
		void setFlag(int8_t flag);
		const ServerConfig& getServerConfig() const;
		int getFd() const;
		unsigned long long getLastAlive() const;
		void updateLastAlive();
};

#endif
