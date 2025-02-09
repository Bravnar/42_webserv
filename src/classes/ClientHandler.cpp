#include "./ClientHandler.hpp"

std::ostream& ClientHandler::fatal(const std::string& msg) { return Logger::fatal(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg); }
std::ostream& ClientHandler::error(const std::string& msg) { return Logger::error(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (" + Convert::ToString(this->socket_fd_) + "): " + msg); }
std::ostream& ClientHandler::warning(const std::string& msg) { return Logger::warning(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (" + Convert::ToString(this->socket_fd_) + "): " + msg); }
std::ostream& ClientHandler::info(const std::string& msg) { return Logger::info(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (" + Convert::ToString(this->socket_fd_) + "): " + msg); }
std::ostream& ClientHandler::debug(const std::string& msg) { return Logger::debug(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (" + Convert::ToString(this->socket_fd_) + "): " + msg); }

static pollfd createPollfd(int fd) {
	pollfd out;
	out.fd = fd;
	out.events = POLLIN | POLLOUT;
	out.revents = 0;
	return out;
}

ClientHandler::ClientHandler(Runtime& runtime, ServerManager& server, int socket_fd, sockaddr_in addr, socklen_t addrlen):	
	socket_fd_(socket_fd),
	runtime_(runtime),
	server_(server),
	address_(addr, addrlen) {
		this->runtime_.getSockets().push_back(createPollfd(this->socket_fd_));
		this->debug("New socket") << std::endl;
}

ClientHandler::ClientHandler(const ClientHandler& copy):
	socket_fd_(-1),	
	runtime_(copy.runtime_),
	server_(copy.server_) {
		Logger::fatal("A client was created by copy. Client constructors by copy aren't inteeded; the class init and deconstructor interacts with runtime!") << std::endl;
}

ClientHandler& ClientHandler::operator=(const ClientHandler& assign) {
	if (this == &assign)
		return *this;
	this->socket_fd_ = -1;
	Logger::fatal("A client was assigned (operator=). Client assignments aren't inteeded; the class init and deconstructor interacts with runtime!") << std::endl;
	return *this;
}

ClientHandler::~ClientHandler() {
	this->debug("Client request deconstructor") << std::endl;
	close(this->socket_fd_);
	delete this->buffer_.requestBuffer;
	delete this->buffer_.fileBuffer;
	{
		bool trigger = false;
		std::vector<pollfd>& sockets_ = this->runtime_.getSockets();
		std::vector<pollfd>::iterator it_sockets = sockets_.begin();
		while (it_sockets != sockets_.end()) {
			if (it_sockets->fd == this->socket_fd_) {
				sockets_.erase(it_sockets);
				trigger = true;
				break;
			}
			it_sockets++;
		}
		if (!trigger)
			this->error("socket not destroyed from Runtime sockets_");
	}
	{
		bool trigger = false;
		std::vector<ClientHandler *>& clients_ = this->runtime_.getClients();
		std::vector<ClientHandler *>::iterator it_clients = clients_.begin();
		while (it_clients != clients_.end()) {
			if (*it_clients == this) {
				it_clients = clients_.erase(it_clients);
				trigger = true;
				break;
			}
			it_clients++;
		}
		if (!trigger)
			this->error("client not destroyed from Runtime clients_");
	}
}

void ClientHandler::fillRequestBuffer_() {
	this->state_.isReading = true;
	char buffer[DF_MAX_BUFFER];
	if (!this->buffer_.requestBuffer)
		this->buffer_.requestBuffer = new std::string("");
	ssize_t bytesRead;
	if ((bytesRead = recv(this->socket_fd_, buffer, DF_MAX_BUFFER, 0)) > 0) {
		this->buffer_.requestBuffer->append(buffer, bytesRead);
	}
	if (bytesRead < 0) { throw std::runtime_error(EXC_SOCKET_READ); }
}

void ClientHandler::fillFileBuffer_(std::ifstream& input) {
	this->buffer_.fileBuffer = new std::string("");
	std::string line;
	while (std::getline(input, line)) {
		this->buffer_.fileBuffer->append(line.append("\n"));
	}
	if (input.bad()) {
		throw std::runtime_error(EXC_FILE_READ);
	}
	input.close();
}

//TODO: Refactor hanlde()
//TODO: Inlude max client body size
/**
 * handle: Handle the client request
 * @attention Nasty code! Needs refactor
 */
void ClientHandler::handle() {
	if (this->state_.isReading)
		throw std::runtime_error("Cannot handle a client in isReading_ state");
	if (!this->state_.isFetched) {
		this->fetch();
		this->debug("Request:") << std::endl << C_ORANGE << this->buffer_.requestBuffer->data() << C_RESET << std::endl;
	}
	
	// generating response
	std::string fileName = this->server_.getConfig().getRoutes()[0].getRoot() + request_.getUrl();
	std::ifstream input(fileName.c_str());
	if (input.is_open()) {
		try {
			this->fillFileBuffer_(input);
		} catch (const std::exception& e) {
			(this->response_ = HttpResponse(500)).sendResp(this->socket_fd_);
			throw;
		}
		(this->response_ = HttpResponse(200, this->buffer_.fileBuffer->data(), this->buffer_.fileBuffer->size() - 1, request_.getUrl())).sendResp(this->socket_fd_);
	} else {
		(this->response_ = HttpResponse(404)).sendResp(this->socket_fd_);
		throw std::runtime_error(EXC_FILE_NF(fileName));
	}
}

/**
 * getSocket: Get client socket
 * @return `int` fd socket reference
 */
int ClientHandler::getSocket() const {
	return this->socket_fd_;
}

const HttpRequest& ClientHandler::fetch() {
	if (this->state_.isFetched)
		return this->request_;
	if (this->state_.isReading) {
		throw std::runtime_error(EXC_FETCHING_BREFORE_READ);
	}
	try {
		this->debug("Request: ") << std::endl << C_ORANGE << this->buffer_.requestBuffer->data() << C_RESET << std::endl;
		this->request_ = HttpRequest(this->buffer_.requestBuffer->data());
		this->state_.isFetched = true;
	} catch(const std::exception& e) {
		const std::string msg = e.what();
		if (msg == EXC_BODY_NEG_SIZE || EXC_BODY_NOLIMITER || EXC_HEADER_NOHOST )
			(this->response_ = HttpResponse(400)).sendResp(this->socket_fd_);
		else
			(this->response_ = HttpResponse(500)).sendResp(this->socket_fd_);
		throw;
	}
	return this->request_;
}

const HttpResponse& ClientHandler::getResponse() const {
	return this->response_;
}

const char *ClientHandler::getClientIp() const {
	return this->address_.clientIp;
}

bool ClientHandler::isFetched() const {
	return this->state_.isFetched;
}

int ClientHandler::readSocket() {
	if (this->state_.isFetched) {
		this->state_.isReading = false;
		return 0;
	}
	try {
		fillRequestBuffer_();
		return (this->state_.isReading);
	} catch(const std::exception& e) {
		this->fatal(e.what()) << std::endl;
		(this->response_ = HttpResponse(500)).sendResp(this->socket_fd_);
		return -1;
	}
}

bool ClientHandler::isReading() const { return this->state_.isReading; }
void ClientHandler::setReading(bool value) { this->state_.isReading = value; }
const ServerManager& ClientHandler::getServer() const { return this->server_; };