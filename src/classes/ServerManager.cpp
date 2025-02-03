#include "./ServerManager.hpp"

static sockaddr_in init_addr_in(int port) {
	struct sockaddr_in addrv4;
	std::memset(&addrv4, 0, sizeof(addrv4));
	inet_pton(AF_INET, "0.0.0.0", &addrv4.sin_addr); // TODO: Define with config manager
	addrv4.sin_family = AF_INET;
	addrv4.sin_port = htons(port);
	return addrv4;
}

int ServerManager::init_() {
	this->server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->server_fd_ < 0) {
		this->isHealthy_ = false;
		Logger::fatal("ServerManager: error on server_fd_ socket: ") << strerror(errno) << std::endl;
		return 1;
	}
	int opt = 1;
	if (setsockopt(this->server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		this->isHealthy_ = false;
		Logger::fatal("ServerManager: error on setsockopt: ") << strerror(errno) << std::endl;
		return 1;
	}
	if (bind(this->server_fd_, this->address_, sizeof(addrv4_)) < 0) {
		this->isHealthy_ = false;
		Logger::fatal("ServerManager: error on binding: ") << strerror(errno) << std::endl;
		return 1;
	}
	// TODO: define connections from config
	if (listen(this->server_fd_, 50)) {
		this->isHealthy_ = false;
		Logger::fatal("ServerManager: error on listen: ") << strerror(errno) << std::endl;
		return 1;
	}
	Logger::info("ServerManager: Server binded on port ") << this->port_ << std::endl;
	Logger::info("ServerManager: You can access it from: http://127.0.0.1:" + std::string(Convert::ToString(this->port_))) << std::endl;
	return 0;
}

ServerManager::ServerManager(): port_(8080), addrv4_(init_addr_in(this->port_)), address_((sockaddr *)&this->addrv4_), isHealthy_(true), max_clients_(500), max_timeout_(2000) {
}

ServerManager::ServerManager(int port): port_(port), addrv4_(init_addr_in(this->port_)), address_((sockaddr *)&this->addrv4_), isHealthy_(true), max_clients_(500), max_timeout_(2000) {
}

ServerManager::ServerManager(const ServerManager& copy): port_(copy.port_), addrv4_(init_addr_in(this->port_)), address_((sockaddr *)&this->addrv4_), isHealthy_(true), max_clients_(500), max_timeout_(2000) {
}

ServerManager& ServerManager::operator=(const ServerManager& assign) {
	if (this == &assign)
		return *this;
	return *this;
}

ServerManager::~ServerManager() {
	Logger::debug("ServerManager: ServerManager deconstructor") << std::endl;
	for(std::vector<ClientHandler *>::iterator it = this->clients_.begin(); it != this->clients_.end(); it++) {
		delete *it;
	}
	this->clients_.clear();
	this->sockets_.clear();
	close(this->server_fd_);
}

bool ServerManager::isHealthy() const {
	return this->isHealthy_;
}

int ServerManager::run() {
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
		if (poll(&this->sockets_[0], this->sockets_.size(), this->max_timeout_) < 0) {
			if (errno == EINTR) {
				Logger::error("ServerManager: poll error: ") << strerror(errno) << std::endl;
				continue;
			}
			else {
				Logger::fatal("ServerManager: poll fatal: ") << strerror(errno) << std::endl;
				this->isHealthy_ = false;
				break;
			}
		}
		if (this->sockets_[0].revents & POLLIN) {
			Logger::debug("ServerManager: Poll server") << std::endl;
			client_socket = accept(this->server_fd_, (sockaddr *)&client_addr, &client_len);
			if (client_socket < 0) {
				Logger::error("ServerManager: error on request accept(): ") << strerror(errno) << std::endl;
				continue;
			}
			this->clients_.push_back(new ClientHandler(*this, client_socket, client_addr, client_len));
			Logger::debug("ServerManager: Accepted new client: ") << client_socket << std::endl;
		}
		for (size_t i = 0; i < this->clients_.size(); i++) {
			if (this->clients_[i]->getSocket().revents & POLLIN) {
				this->clients_[i]->handle();
			}
		}
	}
	return 0;
}

std::vector<pollfd>& ServerManager::getSockets() {
	return this->sockets_;
}

std::vector<ClientHandler *>& ServerManager::getClients() {
	return this->clients_;
}