#include "./Runtime.hpp"

std::ostream& Runtime::fatal(const std::string& msg) { return Logger::fatal("Runtime: " + msg); }
std::ostream& Runtime::error(const std::string& msg) { return Logger::error("Runtime: " + msg); }
std::ostream& Runtime::warning(const std::string& msg) { return Logger::warning("Runtime: " + msg); }
std::ostream& Runtime::info(const std::string& msg) { return Logger::info("Runtime: " + msg); }
std::ostream& Runtime::debug(const std::string& msg) { return Logger::debug("Runtime: " + msg); }

Runtime::Runtime(const std::vector<ServerConfig>& configs) {
	{
		std::vector<ServerManager *> vservers;
		for(std::vector<ServerConfig>::const_iterator it = configs.begin(); it != configs.end(); it++) {
			vservers.push_back(new ServerManager(*it));
		}
		for(std::vector<ServerManager *>::iterator it = vservers.begin(); it != vservers.end(); it++) {
			ServerManager *srv;
			srv = *it;
			try {
				srv->init();
				this->servers_.insert(std::make_pair(srv->getSocket().fd, srv));
				this->sockets_.push_back(srv->getSocket());
			} catch (const std::exception& e) {
				this->fatal("couldn't bind host '") << srv->getConfig().getServerNames()[0] << "' : " << e.what() << std::endl;
			}
		}
	}
}

Runtime::Runtime(const Runtime& copy){
	(void)copy;
	// A runtime is unique
}

Runtime& Runtime::operator=(const Runtime& assign) {
	if (this == &assign)
		return *this;
	return *this;
	// A runtime is unique
}

Runtime::~Runtime() {
	this->debug("deconstructor") << std::endl;

	for(std::vector<ClientHandler *>::iterator it = this->clients_.begin(); it != this->clients_.end(); it++) {
		delete *it;
	}
	this->clients_.clear();
	for(std::map<int, ServerManager *>::iterator it = this->servers_.begin(); it != this->servers_.end(); it++) {
		delete it->second;
	}
	this->servers_.clear();
	for(std::vector<pollfd>::iterator it = this->sockets_.begin(); it != this->sockets_.end(); it++) {
		close(it->fd);
	}
	this->sockets_.clear();
}

void Runtime::checkServers_() {
	int client_socket = -1;
	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	for (size_t i = 0; i < this->servers_.size(); i++) {
		if (this->sockets_[i].revents & POLLIN) {
			this->debug("Poll server '") << this->servers_[this->sockets_[i].fd]->getConfig().getServerNames()[0] << std::endl;
			client_socket = accept(this->sockets_[i].fd, (sockaddr *)&client_addr, &client_len);
			if (client_socket < 0) {
				this->error("error on request accept(): ") << strerror(errno) << std::endl;
				continue;
			}
			this->clients_.push_back(new ClientHandler(*this, *this->servers_[this->sockets_[i].fd], client_socket, client_addr, client_len));
		}
	}
}

void Runtime::checkClients_() {
	for(size_t i = 0; i < this->clients_.size(); i++) {
		for(size_t j = 0; j < this->sockets_.size(); j++) { // only to find his socket
			if (this->sockets_[j].fd == this->clients_[i]->getSocket()) {
				if (this->sockets_[j].revents & POLLIN) {
					try {
						const HttpRequest& req = this->clients_[i]->fetch();
						std::string element = req.getMethod() + " " + req.getUrl() + " " + req.getHttpVersion();
						if (req.isValid()) this->info("Client " + this->clients_[i]->getClientIp() + std::string(" requested ") + element) << std::endl;
						const HttpResponse& resp = this->clients_[i]->getResponse();
						try {
							this->clients_[i]->handle();
							this->info("Response ") << resp.getStatus() << " " << resp.getStatusMsg() << " for " << element << std::endl;
						} catch(const std::exception& httpError) {
							this->error("Response ") << resp.getStatus() << " " << resp.getStatusMsg() << " for " << element << std::endl;
						}
					}
					catch (const std::exception& e) {
						this->error(e.what()) << std::endl;
					}
					delete this->clients_[i];
				}
				break;
			}
		}
	}
}

void Runtime::checkFiles_() {
	// need code
}

void Runtime::runServers() {
	while (true) {
		if (poll(&this->sockets_[0], this->sockets_.size(), 2000) < 0) {
			if (errno == EINTR) {
				this->error("poll error: ") << strerror(errno) << std::endl;
				continue;
			}
			else {
				this->fatal("poll fatal: ") << strerror(errno) << std::endl;
				break;
			}
		}
		this->checkServers_();
		this->checkClients_();
		this->checkFiles_();
	}
	return;
}

void Runtime::closeServers() {
	this->info("Closing server") << std::endl;
	for(std::vector<ClientHandler *>::iterator it = this->clients_.begin(); it != this->clients_.end(); it++) {
		delete *it;
	}
	this->clients_.clear();
	for(std::map<int, ServerManager *>::iterator it = this->servers_.begin(); it != this->servers_.end(); it++) {
		delete it->second;
	}
	this->servers_.clear();
	for(std::vector<pollfd>::iterator it = this->sockets_.begin(); it != this->sockets_.end(); it++) {
		close(it->fd);
	}
	this->sockets_.clear();	
}

std::vector<pollfd>& Runtime::getSockets() {
	return this->sockets_;
}

std::vector<ClientHandler *>& Runtime::getClients() {
	return this->clients_;
}