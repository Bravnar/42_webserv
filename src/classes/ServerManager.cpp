#include "./ServerManager.hpp"

std::ostream& ServerManager::fatal(const std::string& msg) { return Logger::fatal(C_BLUE + this->config_.getServerNames()[0] + C_RESET + ": ServerManager: " + msg); }
std::ostream& ServerManager::error(const std::string& msg) { return Logger::error(C_BLUE + this->config_.getServerNames()[0] + C_RESET + ": ServerManager: " + msg); }
std::ostream& ServerManager::warning(const std::string& msg) { return Logger::warning(C_BLUE + this->config_.getServerNames()[0] + C_RESET + ": ServerManager: " + msg); }
std::ostream& ServerManager::info(const std::string& msg) { return Logger::info(C_BLUE + this->config_.getServerNames()[0] + C_RESET + ": ServerManager: " + msg); }
std::ostream& ServerManager::debug(const std::string& msg) { return Logger::debug(C_BLUE + this->config_.getServerNames()[0] + C_RESET + ": ServerManager: " + msg); }

/**
 * newAddr: Creates a new socket address from specific port and ipv4 interface
 * @param port The port to bind to
 * @param interface The ipv4 interface address to bind to
 * @return New socket address as `sockaddr_in`
 */
static sockaddr_in newAddr(int port, std::string interface) {
	struct sockaddr_in addrv4;
	std::memset(&addrv4, 0, sizeof(addrv4));
	inet_pton(AF_INET, interface.c_str(), &addrv4.sin_addr);
	addrv4.sin_family = AF_INET;
	addrv4.sin_port = htons(port);
	return addrv4;
}

/**
 * init_: Initialize the server by binding it's address and setting internal variable
 * @return `1` if the server couldn't init
 */
int ServerManager::init() {
	this->status_.isHealthy = true;
	this->server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->server_fd_ < 0) {
		this->status_.isHealthy = false;
		throw std::logic_error("server_fd_ socket: " + std::string(strerror(errno)));
	}
	int opt = 1;
	if (setsockopt(this->server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		this->status_.isHealthy = false;
		throw std::logic_error("setsockopt: " + std::string(strerror(errno)));
	}
	if (bind(this->server_fd_, this->address_, sizeof(addrv4_)) < 0) {
		this->status_.isHealthy = false;
		throw std::logic_error("binding " + this->config_.getHost() + ":" + Convert::ToString(this->config_.getPort()) + ": " + std::string(strerror(errno)));
	}
	// TODO: define connections from config
	if (listen(this->server_fd_, this->maxClients_)) {
		this->status_.isHealthy = false;
		throw std::logic_error("error on listen: " + std::string(strerror(errno)));
	}

	this->socket_.fd = this->server_fd_;
	this->socket_.events = POLLIN;
	this->socket_.revents = 0;
	this->info("Virtual host listening on: http://" + (this->config_.getHost() == "0.0.0.0" ? "127.0.0.1" : this->config_.getHost()) + ":" + std::string(Convert::ToString(this->config_.getPort()))) << std::endl;
	this->status_.isRunning = true;
	return 0;
}

ServerManager::ServerManager(const ServerConfig& config, size_t maxClients):
	config_(config),
	addrv4_(newAddr(config.getPort(), config.getHost())),
	address_((sockaddr *)&this->addrv4_),
	routeconfig_(config.getRoutes()),
	maxClients_(maxClients) {
		this->server_fd_ = 0;
		std::memset(&this->status_, 0, sizeof(t_status));
}

ServerManager::ServerManager(const ServerManager& copy):
	config_(copy.config_), addrv4_(copy.addrv4_),
	address_((sockaddr *)&this->addrv4_),
	routeconfig_(copy.routeconfig_),
	maxClients_(copy.maxClients_) {
		this->server_fd_ = 0;
		std::memset(&this->status_, 0, sizeof(t_status));
}

ServerManager& ServerManager::operator=(const ServerManager& assign) {
	if (this == &assign)
		return *this;
	return *this;
}

ServerManager::~ServerManager() {
	this->debug("ServerManager deconstructor") << std::endl;
}

/**
 * isHealthy: Check if the server is healthy
 * @return Health status
 */
bool ServerManager::isHealthy() const {
	return this->status_.isHealthy;
}

/**
 * getSockets: Get server sockets, including `server socket` at position `0`
 * @return A `<pollfd>` socket vector reference
 */
const pollfd& ServerManager::getSocket() const {
	return this->socket_;
}

/**
 * getConfig: Get the current server configuration
 * @return A `ServerConfig` const reference
 */
const ServerConfig& ServerManager::getConfig() const {
	return this->config_;
}

/**
 * getRouteConfig: Get the current server routing configuration
 * @return A `std::vector<RouteConfig>` const reference
 */
const std::vector<RouteConfig>& ServerManager::getRouteConfig() const {
	return this->routeconfig_;
}