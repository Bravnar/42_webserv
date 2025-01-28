#include "./ServerManager.hpp"

static sockaddr_in init_addr_in(int port) {
	struct sockaddr_in addrv4;

	std::memset(&addrv4, 0, sizeof(addrv4));
	addrv4.sin_addr.s_addr = htonl(INADDR_ANY);
	addrv4.sin_family = AF_INET;
	addrv4.sin_port = htons(port);

	return addrv4;
}

int ServerManager::init_() {
	this->server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->server_fd_ < 0) {
		this->isHealthy_ = false;
		Logger::fatal("ServerManager: error on server_fd_ socket: " + static_cast<std::string>(strerror(errno)));
		return 1;
	}
	int opt = 1;
	if (setsockopt(this->server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		this->isHealthy_ = false;
		Logger::fatal("ServerManager: error on setsockopt: " + std::string(strerror(errno)));
		return 1;
	}
	if (bind(this->server_fd_, this->address_, sizeof(addrv4_)) < 0) {
		this->isHealthy_ = false;
		Logger::fatal("ServerManager: error on binding: " + static_cast<std::string>(strerror(errno)));
		return 1;
	}
	// TODO: define connections from config
	if (listen(this->server_fd_, 50)) {
		this->isHealthy_ = false;
		Logger::fatal("ServerManager: error on listen: " + static_cast<std::string>(strerror(errno)));
		return 1;
	}
	Logger::info("ServerManager: Server binded on port " + Convert::ToString(this->port_));
	Logger::info("ServerManager: You can access it from: http://127.0.0.1:" + Convert::ToString(this->port_));
	return 0;
}

ServerManager::ServerManager(): port_(8080), addrv4_(init_addr_in(this->port_)), address_((sockaddr *)&this->addrv4_), isHealthy_(true) {
}

ServerManager::ServerManager(int port): port_(port), addrv4_(init_addr_in(this->port_)), address_((sockaddr *)&this->addrv4_), isHealthy_(true) {
}

ServerManager::ServerManager(const ServerManager& copy): port_(copy.port_), addrv4_(init_addr_in(this->port_)), address_((sockaddr *)&this->addrv4_), isHealthy_(true) {
}

ServerManager& ServerManager::operator=(const ServerManager& assign) {
	if (this == &assign)
		return *this;
	return *this;
}

ServerManager::~ServerManager() {
	Logger::debug("ServerManager: ServerManager deconstructor");
	close(this->server_fd_);
}

bool ServerManager::isHealthy() {
	return this->isHealthy_;
}



int ServerManager::run() {
	if (this->init_())
		return 1;
	int client_socket = -1;
	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	while (true) {
		if ((client_socket = accept(this->server_fd_, (sockaddr*)&client_addr, &client_len)) < 0) {
			this->isHealthy_ = false;
			Logger::error("ServerManager: error on request accept(): " + static_cast<std::string>(strerror(errno)));
			continue;
		}
		ClientHandler client(client_socket, client_addr, client_len);
		client.handle();
	}
	return 0;
}
