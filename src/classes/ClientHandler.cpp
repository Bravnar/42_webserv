#include "./ClientHandler.hpp"

static pollfd makeClientSocket(int fd) {
	pollfd out;
	out.fd = fd;
	out.events = POLLIN;
	out.revents = 0;
	return out;
}

ClientHandler::ClientHandler(ServerManager& server, int client_socket, sockaddr_in client_addr, socklen_t len):
	server_(server),
	addr_(client_addr),
	len_(len) {
		this->server_.getSockets().push_back(makeClientSocket(client_socket));
		this->socket_ = &server.getSockets().back();
		Logger::debug("ClientHandler: New socket fd: " + Convert::ToString(this->socket_->fd));
}

ClientHandler::ClientHandler(const ClientHandler& copy):
	server_(copy.server_),
	addr_(copy.addr_),
	len_(copy.len_),
	socket_(copy.socket_) {
}

ClientHandler::~ClientHandler() {
	Logger::debug("ClientHandler: Client request deconstructor");
	close(this->socket_->fd);
	{
		std::vector<pollfd>& sockets_ = this->server_.getSockets();
		std::vector<pollfd>::iterator it_sockets = sockets_.begin();
		while (it_sockets != sockets_.end()) {
			if (&(*it_sockets) == this->socket_) {
				sockets_.erase(it_sockets);
				break;
			}
			it_sockets++;
		}
	}
	{
		std::vector<ClientHandler *>& clients_ = this->server_.getClients();
		std::vector<ClientHandler *>::iterator it_clients = clients_.begin();
		while (it_clients != clients_.end()) {
			if (*it_clients == this) {
				it_clients = clients_.erase(it_clients);
				break;
			}
			it_clients++;
		}
	}
}

// TODO: This handler is basic and nasty, needs refactor
int ClientHandler::handle() {
	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(this->addr_.sin_addr), client_ip, INET_ADDRSTRLEN); // TODO: May need to replace way of getting ip
	Logger::debug("ClientHandler: handling request from " + static_cast<std::string>(client_ip));
	char buffer[1025] = {0};
	size_t bytes = read(this->socket_->fd, buffer, sizeof(buffer) - 1);
	buffer[bytes] = 0;
	if (bytes)
		Logger::debug("ClientHandler: data:\n" + static_cast<std::string>(C_YELLOW) + buffer + static_cast<std::string>(C_RESET));
	else
		Logger::debug("ClientHandler: no data to read");
	std::string res = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
	write(this->socket_->fd, res.c_str(), res.length());
	delete this;
	return 0;
}

const pollfd& ClientHandler::getSocket() const {
	return *this->socket_;
}