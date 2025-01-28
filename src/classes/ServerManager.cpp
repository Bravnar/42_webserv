#include "./ServerManager.hpp"
#include <iostream>

void ServerManager::init_() {
	std::memset(&this->addrv4_, 0, sizeof(this->addrv4_));
	this->addrv4_.sin_family = AF_INET;
	this->addrv4_.sin_addr.s_addr = htonl(INADDR_ANY);
	this->addrv4_.sin_port = htons(this->port_);
	this->server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
}

ServerManager::ServerManager(): port_(8080), address_((sockaddr *)&this->addrv4_) {
	this->init_();
}

ServerManager::ServerManager(int port): port_(port), address_((sockaddr *)&this->addrv4_) {
	this->init_();
}

ServerManager::ServerManager(const ServerManager& copy): port_(copy.port_), address_((sockaddr *)&this->addrv4_)  {
	this->addrv4_ = copy.addrv4_;
	this->init_();
}

ServerManager& ServerManager::operator=(const ServerManager& assign) {
	if (this == &assign)
		return *this;
	this->addrv4_ = assign.addrv4_;
	this->port_ = assign.port_;
	return *this;
}

ServerManager::~ServerManager() {
	close(this->server_fd_);
}