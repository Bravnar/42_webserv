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
		Logger::debug(C_BLUE + this->server_.getConfig().name + C_RESET + ": ClientHandler: New socket fd: ") << this->socket_->fd << std::endl;
}

ClientHandler::ClientHandler(const ClientHandler& copy):
	server_(copy.server_),
	addr_(copy.addr_),
	len_(copy.len_),
	socket_(copy.socket_) {
}

ClientHandler::~ClientHandler() {
	Logger::debug(C_BLUE + this->server_.getConfig().name + C_RESET + ": ClientHandler: Client request deconstructor") << std::endl;
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

//TODO: Refactor hanlde()
/**
 * handle: Handle the client request
 * @attention Nasty code! Needs refactor
 */
void ClientHandler::handle() {
	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(this->addr_.sin_addr), client_ip, INET_ADDRSTRLEN); // TODO: May need to replace way of getting ip
	Logger::debug(C_BLUE + this->server_.getConfig().name + C_RESET + ": ClientHandler: handling request from ") << client_ip << std::endl;
	size_t buffer_size = this->server_.getConfig().max_buffer;
	char *buffer = new char[buffer_size + 1];
	ssize_t bytes = read(this->socket_->fd, buffer, buffer_size);
	if (bytes > 0) {
		buffer[bytes] = 0;
		Logger::debug(C_BLUE + this->server_.getConfig().name + C_RESET + ": ClientHandler: data:") << std::endl << C_YELLOW << buffer << C_RESET << std::endl;
	}
	else
		Logger::debug(C_BLUE + this->server_.getConfig().name + C_RESET + ": ClientHandler: no data to read") << std::endl;
	std::string res = "HTTP/1.1 200 OK\n";
	int html_file = open("./index.html", O_RDONLY);
	if (html_file <= 0) {
		Logger::warning("Couldn't find") << std::endl;
		res += "Content-Type: text/plain\nContent-Length:24\n\nCouldn't find index.html";
		write(this->socket_->fd, res.c_str(), res.length());
	}
	char buffer_file[2048] = {0};
	ssize_t file_bytes = read(html_file, buffer_file, 2048);
	res += "Content-Type: text/html\nContent-Length:" + Convert::ToString(file_bytes) + "\n\n" + buffer_file;
	write(this->socket_->fd, res.c_str(), res.length());
	delete[] buffer;
	delete this;
}

/**
 * getSocket: Get client socket
 * @return `pollfd` socket reference
 */
const pollfd& ClientHandler::getSocket() const {
	return *this->socket_;
}