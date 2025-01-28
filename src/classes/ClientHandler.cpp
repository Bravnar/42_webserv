#include "./ClientHandler.hpp"

ClientHandler::ClientHandler(int client_socket, sockaddr_in client_addr, socklen_t len):
	client_fd_(client_socket),
	client_addr_(client_addr),
	client_len_(len) {
}

ClientHandler::ClientHandler(const ClientHandler& copy):
	client_fd_(copy.client_fd_),
	client_addr_(copy.client_addr_),
	client_len_(copy.client_len_) {
}

ClientHandler::~ClientHandler() {
	Logger::debug("ClientHandler: Client request deconstructor");
	close(this->client_fd_);
}

// TODO: This handler is basic and nasty, needs refactor
int ClientHandler::handle() {
	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(this->client_addr_.sin_addr), client_ip, INET_ADDRSTRLEN); // TODO: May need to replace way of getting ip
	Logger::debug("ClientHandler: handling request from " + static_cast<std::string>(client_ip));
	char buffer[1025] = {0};
	size_t bytes = read(this->client_fd_, buffer, sizeof(buffer) - 1);
	buffer[bytes] = 0;
	if (bytes)
		Logger::debug("ClientHandler: data:\n" + static_cast<std::string>(C_YELLOW) + buffer + static_cast<std::string>(C_RESET));
	else
		Logger::debug("ClientHandler: no data to read");
	std::string res = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	write(this->client_fd_, res.c_str(), res.length());
	return 0;
}