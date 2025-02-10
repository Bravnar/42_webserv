#include "./Runtime.hpp"

std::ostream& Runtime::fatal(const std::string& msg) { return Logger::fatal("Runtime: " + msg); }
std::ostream& Runtime::error(const std::string& msg) { return Logger::error("Runtime: " + msg); }
std::ostream& Runtime::warning(const std::string& msg) { return Logger::warning("Runtime: " + msg); }
std::ostream& Runtime::info(const std::string& msg) { return Logger::info("Runtime: " + msg); }
std::ostream& Runtime::debug(const std::string& msg) { return Logger::debug("Runtime: " + msg); }

Runtime::Runtime(const std::vector<ServerConfig>& configs, size_t maxClients) {
	pipe(this->syncPipe_);
	this->syncPoll_.events = POLLIN | POLLOUT;
	this->syncPoll_.revents = 0;
	this->syncPoll_.fd = this->syncPipe_[0];
	
	// reserve for: syncPipe, serverFds + serverFds * maxClients (serverFds is not calculated; using configs.size() instead)
	this->sockets_.reserve(sizeof(pollfd) * (1 + configs.size() * (1 + maxClients)));

	this->sockets_.push_back(this->syncPoll_);
	this->isSyncing_ = false;
	{
		std::vector<ServerManager *> vservers;
		for(std::vector<ServerConfig>::const_iterator it = configs.begin(); it != configs.end(); it++) {
			vservers.push_back(new ServerManager(*it, maxClients));
		}
		for(std::vector<ServerManager *>::iterator it = vservers.begin(); it != vservers.end(); it++) {
			ServerManager *srv;
			srv = *it;
			try {
				srv->init();
				this->servers_.insert(std::make_pair(srv->getSocket().fd, srv));
				this->sockets_.push_back(srv->getSocket());
			} catch (const std::exception& e) {
				this->fatal("'") << srv->getConfig().getServerNames()[0] << "' : " << e.what() << std::endl;
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

	while (this->clients_.size()) {
		this->debug("destroying ") << this->clients_[0]->getSocket() << std::endl;
		delete this->clients_[0];
	}
	this->clients_.clear();
	for(std::map<int, ServerManager *>::iterator it = this->servers_.begin(); it != this->servers_.end(); it++) {
		delete it->second;
	}
	this->servers_.clear();
	for(std::vector<pollfd>::iterator it = this->sockets_.begin(); it != this->sockets_.end(); it++) {
		close(it->fd);
	}
	close(this->syncPipe_[1]);
	this->sockets_.clear();
}

void Runtime::checkServers_() {
	int client_socket = -1;
	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	// starting at idx 1 since idx 0 is reserved to syncPipe
	for (size_t i = 1; i < this->servers_.size() + 1; i++) {
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
		ClientHandler *client = this->clients_[i];
		pollfd *socket = this->getSocket(client->getSocket());
		if (!socket) {
			this->fatal("lost socket for client ") << client->getClientIp() << std::endl;
			delete client;
			continue;
		}
		if (!(socket->events & POLLOUT)) {
			this->error("lost client ") << client->getClientIp() << std::endl;
			delete client;
			continue;
		}
		if (socket->revents & POLLIN) {
			this->debug("pollin client (") << client->getSocket() << ")" << std::endl;
			if(!client->isReading()) client->setReading(true);
			client->readSocket();
			if (client->isDead()) {
				this->fatal("throwing client ") << client->getClientIp() << std::endl;
				delete client;
			}
			continue;
		} else if (client->isReading()) {
			this->debug("pollin end client (") << client->getSocket() << ")" << std::endl;
			client->buildRequest();
			continue;
		}
		if (socket->events & POLLOUT && client->isFetched()) {
			this->debug("pollout client (") << client->getSocket() << ")" << std::endl;
			if (!client->isSending()) {
				client->buildResponse(HttpResponse(client->getRequest()));
			}				
			client->sendResponse();
			if (client->isSent()) {
				delete client;
			}
			continue;
		}
	}
}

void Runtime::runServers() {
	if (this->servers_.empty()) {
		this->error("No binded servers to run") << std::endl;
		return;
	}
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
		if (this->sockets_[0].revents & POLLIN) {
			char flush;
			read (this->syncPipe_[0], &flush, 1);
			this->isSyncing_ = false;
		}
		this->checkServers_();
		this->checkClients_();
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

void Runtime::Sync() {
	if (this->isSyncing_) return;
	this->isSyncing_ = true;
	write(this->syncPipe_[1], "\0", 1);
	this->sockets_[0].fd = this->syncPipe_[0];
}

pollfd *Runtime::getSocket(int socket_fd_) {
	for(size_t i = 0; i < this->sockets_.size(); i++) {
		if(socket_fd_ == this->sockets_[i].fd) {
			return &this->sockets_[i];
		}
	}
	return 0;
}