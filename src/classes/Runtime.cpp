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
				this->servers_[srv->getSocket().fd] =  srv;
				this->sockets_.push_back(srv->getSocket());
			} catch (const std::exception& e) {
				this->fatal("'") << srv->getConfig().getServerNames()[0] << "' : " << e.what() << std::endl;
				delete *it;
			}
		}
	}
}

Runtime::Runtime(const Runtime& copy){
	(void)copy;
}

Runtime& Runtime::operator=(const Runtime& assign) {
	if (this == &assign)
		return *this;
	return *this;
}

Runtime::~Runtime() {
	#if LOGGER_DEBUG > 0
		this->debug("deconstructor") << std::endl;
	#endif
	this->handleExit_();
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
		this->checkSyncPipe_();
		this->checkServers_();
		this->checkClients_();
	}
	return;
}

void Runtime::closeServers() {
	this->info("Closing server") << std::endl;
	this->handleExit_();
}

void Runtime::handleExit_() {
	while (this->clients_.size()) {
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

void Runtime::checkSyncPipe_() {
	if (this->sockets_[0].revents & POLLIN) {
		char flush;
		read (this->syncPipe_[0], &flush, 1);
		this->isSyncing_ = false;
	}
}

void Runtime::checkClients_() {
	for(size_t i = 0; i < this->clients_.size(); i++) {
		ClientHandler *client = this->clients_[i];
		pollfd *socket = this->getSocket_(client->getSocket());
		if (!socket) {
			this->fatal("lost socket for client ") << client->getClientIp() << " (fd: " << client->getSocket() << ")" << std::endl;
			delete client;
			continue;
		}
		if (this->handleClientPollin_(client, socket) < 0 || this->handleClientPollout_(client, socket)) {
			if (this->clients_[i] != client)
				i--;
			continue;
		}
	}
}

void Runtime::checkServers_() {
	int client_socket = -1;
	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	// starting at idx 1 since idx 0 is reserved to syncPipe
	for (size_t i = 1; i < this->servers_.size() + 1; i++) {
		if (this->sockets_[i].revents & POLLIN) {
			client_socket = accept(this->sockets_[i].fd, (sockaddr *)&client_addr, &client_len);
			if (client_socket < 0) {
				this->error("error on request accept(): ") << strerror(errno) << std::endl;
				continue;
			}
			this->clients_.push_back(new ClientHandler(*this, *this->servers_[this->sockets_[i].fd], client_socket, client_addr, client_len));
		}
	}
}

int Runtime::handleClientPollin_(ClientHandler *client, pollfd *socket) {
	if (socket->revents & POLLIN) {
		#if LOGGER_DEBUG > 0
			this->debug("pollin client (fd: ") << client->getSocket() << ")" << std::endl;
		#endif
		if (client->isFetched()) {
			std::ostream& stream = this->warning("throwing sticky client");
			#if LOGGER_DEBUG > 0
				stream << " (fd: " << client->getSocket() << ")";
			#endif
			stream << std::endl;
			delete client;
			return -1;
		}
		client->setFetched(false);
		if(!client->isReading()) client->setReading(true);
		try {
			client->readSocket();
		} catch (const std::exception& e) {
			client->buildResponse(HttpResponse(500));
			#if LOGGER_DEBUG > 0
				this->debug("client ") << client->getSocket() << ": " << e.what() << std::endl;
			#endif
			return 1;
		}
		if (client->isDead()) {
			this->fatal("throwing client ") << client->getClientIp() << " (fd: " << client->getSocket() << ")" << std::endl;
			delete client;
			return -1;
		}
	} else if (client->isReading()) {
		#if LOGGER_DEBUG > 0
			this->debug("pollin end client (fd: ") << client->getSocket() << ")" << std::endl;
		#endif
		try {
			client->buildRequest();
			std::ostream& stream = this->info("") << C_BLUE << client->getServer().getConfig().getServerNames()[0] << C_RESET << ": Request "
				<< client->getRequest().getMethod() << " " << client->getRequest().getUrl()
				<< " client " << client->getClientIp();
			if (LOGGER_DEBUG)
				stream << " (fd: " << client->getSocket() << ")";
			stream << std::endl;
		} catch (const std::exception& e) {
			std::string msg(e.what());
			if (msg == EXC_BODY_NEG_SIZE || msg == EXC_BODY_NOLIMITER || msg == EXC_HEADER_NOHOST)
				client->buildResponse(HttpResponse(400));
			else
				client->buildResponse(HttpResponse(500));
			#if LOGGER_DEBUG > 0
				this->debug("client ") << client->getSocket() << ": " << e.what() << std::endl;
			#endif
			return 1;
		}
	}
	return 0;
}

int Runtime::handleClientPollout_(ClientHandler *client, pollfd *socket) {
	if (socket->events & POLLOUT && client->isFetched()) {
		#if LOGGER_DEBUG > 0
			this->debug("pollout client (fd: ") << client->getSocket() << ")" << std::endl;
		#endif
		if (!client->hasResponse() && !client->isSending()) {
			client->buildResponse(HttpResponse(client->getRequest()));
		}
		try {
			client->sendResponse();
		} catch(const std::exception& e) {
			this->fatal("throwing client: ") << e.what() << std::endl;
			delete client;
			return -1;
		}
		if (client->isSent()) {
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
					*stream << " (fd: " << client->getSocket() << ")";
			#endif
			*stream << std::endl;
			delete client;
			return 1;
		}
	} else if (!(socket->events & POLLOUT)) {
		this->error("lost client ") << client->getClientIp() << " (fd: " << client->getSocket() << ")" << std::endl;
		delete client;
		return -1;
	}
	return 0;
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

pollfd *Runtime::getSocket_(int socket_fd_) {
	for(size_t i = 0; i < this->sockets_.size(); i++) {
		if(socket_fd_ == this->sockets_[i].fd) {
			return &this->sockets_[i];
		}
	}
	return 0;
}