#include "./Runtime.hpp"

std::ostream& Runtime::fatal(const std::string& msg) { return Logger::fatal("Runtime: " + msg); }
std::ostream& Runtime::error(const std::string& msg) { return Logger::error("Runtime: " + msg); }
std::ostream& Runtime::warning(const std::string& msg) { return Logger::warning("Runtime: " + msg); }
std::ostream& Runtime::info(const std::string& msg) { return Logger::info("Runtime: " + msg); }
std::ostream& Runtime::debug(const std::string& msg) { return Logger::debug("Runtime: " + msg); }

Runtime::Runtime(const std::vector<ServerConfig>& configs, size_t maxClients) {	
	this->sockets_.reserve(sizeof(pollfd) * (1 + configs.size() * (1 + maxClients)));

	pipe(this->syncPipe_);
	this->syncPoll_.events = POLLIN;
	this->syncPoll_.revents = 0;
	this->syncPoll_.fd = this->syncPipe_[0];
	this->sockets_.push_back(this->syncPoll_);
	this->isSyncing_ = false;
	this->initializeServers_(configs, maxClients);
}

void Runtime::initializeServers_(const std::vector<ServerConfig>& configs, size_t maxClients) {
	for(std::vector<ServerConfig>::const_iterator config = configs.begin(); config != configs.end(); config++) {
		this->servers_.push_back(ServerManager(*config, maxClients));
	}
	for(std::vector<ServerManager>::iterator server = this->servers_.begin(); server != this->servers_.end(); server++) {
		try {
			server->init();
			this->servers_map_[server->getSocket().fd] = &*server;
			this->sockets_.push_back(server->getSocket());
		} catch (const std::exception& e) {
			this->fatal("'") << server->getConfig().getServerNames()[0] << "' : " << e.what() << std::endl;
		}
	}
}

Runtime::~Runtime() {
    #if LOGGER_DEBUG > 0
        debug("deconstructor") << std::endl;
    #endif
    handleExit_();
}

Runtime& Runtime::operator=(const Runtime& assign) {
	if (this == &assign)
		return *this;
	return *this;
}

void Runtime::runServers() {
	if (this->servers_map_.empty()) {
		this->error("No binded servers to run") << std::endl;
		return;
	}

	signal(SIGPIPE, SIG_IGN);
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
		this->checkSyncPipeSocket_();
		this->checkServersSocket_();
		this->checkClientsSockets_();
	}
	signal(SIGPIPE, SIG_DFL);
	return;
}

void Runtime::closeServers() {
	this->info("Closing server") << std::endl;
	this->handleExit_();
}

void Runtime::handleExit_() {
	while (!this->clients_.empty()) {
		delete &*this->clients_.begin();
	}
	this->clients_.clear();
	this->servers_.clear();
	this->servers_map_.clear();
	for(std::vector<pollfd>::iterator it = this->sockets_.begin(); it != this->sockets_.end(); it++) {
		close(it->fd);
	}
	close(this->syncPipe_[1]);
	close(this->syncPipe_[0]);
	this->sockets_.clear();
}

void Runtime::checkSyncPipeSocket_() {
	if (this->sockets_[0].revents & POLLIN) {
		char flush;
		read (this->syncPipe_[0], &flush, 1);
		this->isSyncing_ = false;
	}
}

void Runtime::checkClientsSockets_() {
	// starting at idx 1 + servers size since 0 and next are reserved
	for(size_t i = 1 + this->servers_map_.size(); i < this->sockets_.size(); i++) {
		#if LOGGER_DEBUG > 0
			Logger::debug("client alive") << std::endl;
		#endif
		if(this->clients_.find(this->sockets_[i].fd) != this->clients_.end()) {
			if (this->handleClientPollin_(this->clients_[this->sockets_[i].fd], &this->sockets_[i])
				|| this->handleClientPollout_(this->clients_[this->sockets_[i].fd], &this->sockets_[i])) {
					i--;
					continue;
			}
		}
	}
}

void Runtime::checkServersSocket_() {
	// starting at idx 1 since idx 0 is reserved to syncPipe
	for (size_t i = 1; i < this->servers_map_.size() + 1; i++) {
		if (this->sockets_[i].revents & POLLIN) {
			int socket_fd = -1;
			sockaddr_in client_addr;
			socklen_t client_len = sizeof(client_addr);
			socket_fd = accept(this->sockets_[i].fd, (sockaddr *)&client_addr, &client_len);
			if (socket_fd < 0) {
				this->error("error on request accept(): ") << strerror(errno) << std::endl;
				continue;
			}
			this->clients_[socket_fd] = new ClientHandler(*this, *this->servers_map_[this->sockets_[i].fd], socket_fd, client_addr, client_len);
		}
	}
}

void Runtime::handleRequest_(ClientHandler *client) {
	std::ostream& stream = this->info("") << C_BLUE << client->getServer().getConfig().getServerNames()[0] << C_RESET << ": Request "
		<< client->getRequest().getMethod() << " " << client->getRequest().getUrl()
		<< " client " << client->getClientIp();
	#if LOGGER_DEBUG > 0
		stream << " (fd: " << client->getFd() << ")";
	#endif
	stream << std::endl;
}

void Runtime::handleRequest_(ClientHandler *client, const std::exception *e) {
	std::string msg(e->what());
	if (msg == EXC_INVALID_RL || msg == EXC_BODY_NEG_SIZE || msg == EXC_BODY_NOLIMITER || msg == EXC_HEADER_NOHOST)
		client->buildResponse(HttpResponse(client->getRequest(), 400));
	else {
		client->buildResponse(HttpResponse(client->getRequest(), 500));
	}
	#if LOGGER_DEBUG > 0
		this->debug("client ") << client->getFd() << ": " << e->what() << std::endl;
	#endif
}

int Runtime::handleClientPollin_(ClientHandler *client, pollfd *socket) {
	if (socket->revents & POLLIN) {
		if(!(client->getFlags() & READING)) client->setFlag(READING);
		#if LOGGER_DEBUG > 0
			this->debug("pollin client (fd: ") << client->getFd() << ")" << std::endl;
		#endif
		if (client->getFlags() & FETCHED) {
			#if LOGGER_DEBUG > 0
				this->debug("throwing sticky client") << " (fd: " << client->getFd() << ")" << std::endl;
			#endif
			delete client;
			return -1;
		}
		try {
			client->readSocket();
		} catch (const std::exception& e) {
			client->buildResponse(HttpResponse(client->getRequest(), 500));
			#if LOGGER_DEBUG > 0
				this->debug("client ") << client->getFd() << ": " << e.what() << std::endl;
			#endif
			return 1;
		}
	} else if (client->getFlags() & READING) {
		socket->events = POLLOUT;
		#if LOGGER_DEBUG > 0
			this->debug("pollin end client (fd: ") << client->getFd() << ")" << std::endl;
		#endif
		try {
			client->buildRequest();
			this->handleRequest_(client);
		} catch (const std::exception& e) {
			this->handleRequest_(client, &e);
			return 1;
		}
	}
	return 0;
}

void Runtime::logResponse_(ClientHandler *client) {
	std::ostream *stream = 0;
	if (client->getResponse().getStatus() < 300) { stream = &this->info(""); }
	else if (client->getResponse().getStatus() < 400) { stream = &this->warning(""); }
	else if (client->getResponse().getStatus() < 500) { stream = &this->error(""); }
	else { stream = &this->fatal(""); }
	*stream << C_BLUE << client->getServer().getConfig().getServerNames()[0] << C_RESET << ": " << client->getResponse().getResLine();
	if (!client->getRequest().getReqLine().empty())
			*stream << " for " << client->getRequest().getMethod() << " " << client->getRequest().getUrl();
	*stream << " client " << client->getClientIp();
	#if LOGGER_DEBUG > 0
		if (LOGGER_DEBUG)
			*stream << " (fd: " << client->getFd() << ")";
	#endif
	*stream << std::endl;
}

int Runtime::handleClientPollout_(ClientHandler *client, pollfd *socket) {
	if (socket->revents & POLLOUT && client->getFlags() & FETCHED) {
		#if LOGGER_DEBUG > 0
			this->debug("pollout client (fd: ") << client->getFd() << ")" << std::endl;
		#endif
		try {
			client->sendResponse();
		} catch(const std::exception& e) {
			this->fatal("throwing client: ") << e.what() << std::endl;
			delete client;
			return -1;
		}
		if (client->getFlags() & SENT) {
			this->logResponse_(client);
			if (client->getResponse().getHeaders()[H_CONNECTION] != "keep-alive") {
				delete client;
				return 1;
			}
			client->flush();
			socket->events = POLLIN;
		}
	}
	return 0;
}

std::vector<pollfd>& Runtime::getSockets() {
	return this->sockets_;
}

std::map<int, ClientHandler *>& Runtime::getClients() {
	return this->clients_;
}

void Runtime::Sync() {
	if (this->isSyncing_) return;
	this->isSyncing_ = true;
	write(this->syncPipe_[1], "\0", 1);
	this->sockets_[0].fd = this->syncPipe_[0];
}

pollfd *Runtime::getSocket_(int socket_fd_) {
	for(size_t i = 0; i < this->sockets_.size(); i++) {
		if(socket_fd_ == this->sockets_[i].fd) {
			return &this->sockets_[i];
		}
	}
	return 0;
}