#include "./ServerManager.hpp"

std::ostream& ServerManager::fatal(const std::string& msg) { return Logger::fatal(C_BLUE + this->config_.name + C_RESET + ": ServerManager: " + msg); }
std::ostream& ServerManager::error(const std::string& msg) { return Logger::error(C_BLUE + this->config_.name + C_RESET + ": ServerManager: " + msg); }
std::ostream& ServerManager::warning(const std::string& msg) { return Logger::warning(C_BLUE + this->config_.name + C_RESET + ": ServerManager: " + msg); }
std::ostream& ServerManager::info(const std::string& msg) { return Logger::info(C_BLUE + this->config_.name + C_RESET + ": ServerManager: " + msg); }
std::ostream& ServerManager::debug(const std::string& msg) { return Logger::debug(C_BLUE + this->config_.name + C_RESET + ": ServerManager: " + msg); }

/**
 * newAddr: Creates a new socket address from specific port and ipv4 interface
 * @param port The port to bind to
 * @param interface The ipv4 interface address to bind to
 * @return New socket address as `sockaddr_in`
 */
static sockaddr_in newAddr(int port, std::string interface) {
	struct sockaddr_in addrv4;
	std::memset(&addrv4, 0, sizeof(addrv4));
	inet_pton(AF_INET, interface.c_str(), &addrv4.sin_addr); // TODO: Define with config manager
	addrv4.sin_family = AF_INET;
	addrv4.sin_port = htons(port);
	return addrv4;
}

/**
 * init_: Initialize the server by binding it's address and setting internal variable
 * @return `1` if the server couldn't init
 */
int ServerManager::init_() {
	this->server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->server_fd_ < 0) {
		this->status_.isHealthy = false;
		this->fatal("error on server_fd_ socket: ") << strerror(errno) << std::endl;
		return 1;
	}
	int opt = 1;
	if (setsockopt(this->server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		this->status_.isHealthy = false;
		this->fatal("error on setsockopt: ") << strerror(errno) << std::endl;
		return 1;
	}
	if (bind(this->server_fd_, this->address_, sizeof(addrv4_)) < 0) {
		this->status_.isHealthy = false;
		this->fatal("error on binding: ") << strerror(errno) << std::endl;
		return 1;
	}
	// TODO: define connections from config
	if (listen(this->server_fd_, 50)) {
		this->status_.isHealthy = false;
		this->fatal("error on listen: ") << strerror(errno) << std::endl;
		return 1;
	}
	this->info("Server binded on port ") << this->config_.port << std::endl;
	this->info("You can access it from: http://" + (this->config_.interface == "0.0.0.0" ? "127.0.0.1" : this->config_.interface) + ":" + std::string(Convert::ToString(this->config_.port))) << std::endl;
	this->status_.isRunning = true;
	return 0;
}

ServerManager::ServerManager(): addrv4_(newAddr(DF_PORT, DF_INTERFACE)), address_((sockaddr *)&this->addrv4_) {
	this->server_fd_ = 0;
	std::memset(&this->status_, 0, sizeof(t_status));
	this->config_.name = "example";
	this->config_.interface = DF_INTERFACE;
	this->config_.port = DF_PORT;
	this->config_.max_clients = 500;
	this->config_.max_buffer = DF_MAX_BUFFER;
}

ServerManager::ServerManager(int port): addrv4_(newAddr(port, DF_INTERFACE)), address_((sockaddr *)&this->addrv4_) {
	this->server_fd_ = 0;
	std::memset(&this->status_, 0, sizeof(t_status));
	this->config_.name = "example";
	this->config_.interface = DF_INTERFACE;
	this->config_.port = port;
	this->config_.max_clients = 500;
	this->config_.max_buffer = DF_MAX_BUFFER;
}

ServerManager::ServerManager(const ServerManager& copy): addrv4_(newAddr(copy.config_.port, copy.config_.interface)), address_((sockaddr *)&this->addrv4_) {
	this->server_fd_ = 0;
	std::memset(&this->status_, 0, sizeof(t_status));
	this->config_.name = copy.config_.name + " copy";
	this->config_.interface = copy.config_.interface;
	this->config_.port = copy.config_.port;
	this->config_.max_clients = copy.config_.max_clients;
	this->config_.max_buffer = copy.config_.max_buffer;
}

ServerManager& ServerManager::operator=(const ServerManager& assign) {
	if (this == &assign)
		return *this;
	return *this;
}

ServerManager::~ServerManager() {
	this->debug("ServerManager deconstructor") << std::endl;
	this->closeServer();
}

/**
 * isHealthy: Check if the server is healthy
 * @return Health status
 */
bool ServerManager::isHealthy() const {
	return this->status_.isHealthy;
}

/**
 * newAddr: Starts the server
 * @return `1` if something went wrong
 */
int ServerManager::runServer() {
	if (this->init_())
		return 1;
	int client_socket = -1;
	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	pollfd server_socket;
	server_socket.fd = this->server_fd_;
	server_socket.events = POLLIN;
	server_socket.revents = 0;
	this->sockets_.push_back(server_socket);
	while (true) {
		if (poll(&this->sockets_[0], this->sockets_.size(), 2000) < 0) {
			if (errno == EINTR) {
				this->error("poll error: ") << strerror(errno) << std::endl;
				continue;
			}
			else {
				this->fatal("poll fatal: ") << strerror(errno) << std::endl;
				this->status_.isHealthy = false;
				break;
			}
		}
		if (this->sockets_[0].revents & POLLIN) {
			this->debug("Poll server") << std::endl;
			client_socket = accept(this->server_fd_, (sockaddr *)&client_addr, &client_len);
			if (client_socket < 0) {
				this->error("error on request accept(): ") << strerror(errno) << std::endl;
				continue;
			}
			this->clients_.push_back(new ClientHandler(*this, client_socket, client_addr, client_len));
			this->debug("Accepted new client: ") << client_socket << std::endl;
		}
		for (size_t i = 0; i < this->clients_.size(); i++) {
			if (this->clients_[i]->getSocket().revents & POLLIN) {
				this->clients_[i]->handle();
			}
		}
	}
	return 0;
}

/**
 * closeServer: Will close and clear the server if it's running 
 */
void ServerManager::closeServer() {
	if (!this->status_.isRunning)
		return;
	this->info("Closing server") << std::endl;
	this->status_.isRunning = false;
	for(std::vector<ClientHandler *>::iterator it = this->clients_.begin(); it != this->clients_.end(); it++) {
		delete *it;
	}
	this->clients_.clear();
	this->sockets_.clear();
	if (this->server_fd_)
		close(this->server_fd_);
}

/**
 * getSockets: Get server sockets, including `server socket` at position `0`
 * @return A `<pollfd>` socket vector reference
 */
std::vector<pollfd>& ServerManager::getSockets() {
	return this->sockets_;
}

/**
 * getClients: Get the current clients communicating with the server
 * @return A `<ClintHandler *>` vector reference
 */
std::vector<ClientHandler *>& ServerManager::getClients() {
	return this->clients_;
}

const t_config ServerManager::getConfig() const {
	return this->config_;
}