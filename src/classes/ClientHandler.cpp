#include "./ClientHandler.hpp"

static pollfd makeClientSocket(int fd) {
	pollfd out;
	out.fd = fd;
	return out;
}

ClientHandler::ClientHandler(ServerManager& server, int client_socket, sockaddr_in client_addr, socklen_t len):
	server_(server),
	socket_(makeClientSocket(client_socket)),
	addr_(client_addr),
	len_(len) {
		Logger::debug("ClientHandler: New socket fd: " + Convert::ToString(this->socket_.fd));
		this->server_.getSockets().push_back(this->socket_);
}

ClientHandler::ClientHandler(const ClientHandler& copy):
	server_(copy.server_),
	socket_(copy.socket_),
	addr_(copy.addr_),
	len_(copy.len_) {
}

// TODO: make it look better
ClientHandler::~ClientHandler() {
	Logger::debug("ClientHandler: Client request deconstructor");
	std::vector<pollfd>& sockets_ = this->server_.getSockets();
	std::vector<pollfd>::iterator it = sockets_.begin();
	while (it != sockets_.end()) {
		if (it->fd == this->socket_.fd) {
			sockets_.erase(it);
			break;
		}
		it++;
	}
	close(this->socket_.fd);
}

// TODO: This handler is basic and nasty, needs refactor
int ClientHandler::handle() {
	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(this->addr_.sin_addr), client_ip, INET_ADDRSTRLEN); // TODO: May need to replace way of getting ip
	Logger::debug("ClientHandler: handling request from " + static_cast<std::string>(client_ip));
	char buffer[1025] = {0};
	size_t bytes = read(this->socket_.fd, buffer, sizeof(buffer) - 1);
	buffer[bytes] = 0;
	if (bytes)
		Logger::debug("ClientHandler: data:\n" + static_cast<std::string>(C_YELLOW) + buffer + static_cast<std::string>(C_RESET));
	else
		Logger::debug("ClientHandler: no data to read");
	std::string res = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	write(this->socket_.fd, res.c_str(), res.length());
	return 0;
}