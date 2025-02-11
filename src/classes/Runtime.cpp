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
	{
		for(std::vector<ServerConfig>::const_iterator it = configs.begin(); it != configs.end(); it++) {
			this->servers_.push_back(ServerManager(*it, maxClients));
		}
		for(std::vector<ServerManager>::iterator it = this->servers_.begin(); it != this->servers_.end(); it++) {
			try {
				it->init();
				this->servers_map_.insert(std::make_pair(it->getSocket().fd, &*it));
				this->sockets_.push_back(it->getSocket());
			} catch (const std::exception& e) {
				this->fatal("'") << it->getConfig().getServerNames()[0] << "' : " << e.what() << std::endl;
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
	if (this->servers_map_.empty()) {
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
	this->servers_.clear();
	this->servers_map_.clear();
	for(std::vector<pollfd>::iterator it = this->sockets_.begin(); it != this->sockets_.end(); it++) {
		close(it->fd);
	}
	close(this->syncPipe_[1]);
	close(this->syncPipe_[0]);
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
		pollfd *socket = this->getSocket_(client->getFd());
		if (!socket) {
			this->fatal("lost socket for client ") << client->getClientIp() << " (fd: " << client->getFd() << ")" << std::endl;
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
	int socket_fd = -1;
	sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	// starting at idx 1 since idx 0 is reserved to syncPipe
	for (size_t i = 1; i < this->servers_map_.size() + 1; i++) {
		if (this->sockets_[i].revents & POLLIN) {
			socket_fd = accept(this->sockets_[i].fd, (sockaddr *)&client_addr, &client_len);
			if (socket_fd < 0) {
				this->error("error on request accept(): ") << strerror(errno) << std::endl;
				continue;
			}
			this->clients_.push_back(new ClientHandler(*this, *this->servers_map_[this->sockets_[i].fd], socket_fd, client_addr, client_len));
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
		if(!client->isReading()) client->setReading(true);
		#if LOGGER_DEBUG > 0
			this->debug("pollin client (fd: ") << client->getFd() << ")" << std::endl;
		#endif
		if (client->isFetched()) {
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
	} else if (client->isReading()) {
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
	if (socket->revents & POLLOUT && client->isFetched()) {
		#if LOGGER_DEBUG > 0
			this->debug("pollout client (fd: ") << client->getFd() << ")" << std::endl;
		#endif
		if (!client->hasResponse() && !client->isSending()) {
			client->buildResponse(HttpResponse(client->getRequest()));
		}
		signal(SIGPIPE, SIG_IGN);
		try {
			client->sendResponse();
		} catch(const std::exception& e) {
			this->fatal("throwing client: ") << e.what() << std::endl;
			delete client;
			return -1;
		}
		signal(SIGPIPE, SIG_DFL);
		if (client->isSent()) {
			this->logResponse_(client);
			std::map<std::string, std::string>& headers = client->getResponse().getHeaders();
			if (headers.find(H_CONNECTION) == headers.end() || headers[H_CONNECTION] != "keep-alive") {
				delete client;
				return -1;
			}
			client->flush();
			socket->events = POLLIN;
			return 1;
		}
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