#include "./ClientHandler.hpp"

std::ostream& ClientHandler::fatal(const std::string& msg) { return Logger::fatal(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_) + "): " + msg); }
std::ostream& ClientHandler::error(const std::string& msg) { return Logger::error(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (" + Convert::ToString(this->socket_) + "): " + msg); }
std::ostream& ClientHandler::warning(const std::string& msg) { return Logger::warning(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (" + Convert::ToString(this->socket_) + "): " + msg); }
std::ostream& ClientHandler::info(const std::string& msg) { return Logger::info(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (" + Convert::ToString(this->socket_) + "): " + msg); }
std::ostream& ClientHandler::debug(const std::string& msg) { return Logger::debug(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (" + Convert::ToString(this->socket_) + "): " + msg); }

static pollfd makeClientSocket(int fd) {
	pollfd out;
	out.fd = fd;
	out.events = POLLIN;
	out.revents = 0;
	return out;
}

ClientHandler::ClientHandler(Runtime& runtime, ServerManager& server, int client_socket, sockaddr_in client_addr, socklen_t len):
	runtime_(runtime),
	server_(server),
	addr_(client_addr),
	len_(len),
	headers_(0),
	fileBuffer_(0),
	fetched_(false) {
		runtime.getSockets().push_back(makeClientSocket(client_socket));
		this->socket_ = client_socket;
		this->debug("New socket") << std::endl;
}

ClientHandler::ClientHandler(const ClientHandler& copy):
	runtime_(copy.runtime_),
	server_(copy.server_),
	addr_(copy.addr_),
	len_(copy.len_),
	socket_(copy.socket_),
	headers_(copy.headers_),
	fileBuffer_(copy.fileBuffer_),
	fetched_(copy.fileBuffer_),
	req_(copy.req_) {
}

ClientHandler& ClientHandler::operator=(const ClientHandler& assign) {
	if (this == &assign)
		return *this;
	this->runtime_ = assign.runtime_;
	this->server_ = assign.server_;
	this->socket_ = assign.socket_;
	this->headers_ = assign.headers_;
	this->fileBuffer_ = assign.fileBuffer_;
	this->fetched_ = assign.fileBuffer_;
	this->req_ = assign.req_;
	return *this;
}

ClientHandler::~ClientHandler() {
	this->debug("Client request deconstructor") << std::endl;
	close(this->socket_);
	delete this->headers_;
	delete this->fileBuffer_;
	{
		bool trigger = false;
		std::vector<pollfd>& sockets_ = this->runtime_.getSockets();
		std::vector<pollfd>::iterator it_sockets = sockets_.begin();
		while (it_sockets != sockets_.end()) {
			if (it_sockets->fd == this->socket_) {
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

void ClientHandler::loadHeaders_() {
	char buffer[DF_MAX_BUFFER];
	this->headers_ = new std::string("");
	ssize_t bytesRead;
	size_t totalBytesRead = 0;
	while ((bytesRead = recv(this->socket_, buffer, DF_MAX_BUFFER, 0)) > 0) {
		this->headers_->append(buffer, bytesRead);
		totalBytesRead += bytesRead;
		if (bytesRead < DF_MAX_BUFFER)
			break;
	}
	if (bytesRead < 0) {
		throw std::runtime_error("Error reading from socket");
	}
}

void ClientHandler::buildBody_(int fileFd) {
	char buffer[DF_MAX_BUFFER];
	this->fileBuffer_ = new std::string("");
	ssize_t bytesRead;
	size_t totalBytesRead = 0;
	while ((bytesRead = read(fileFd, buffer, DF_MAX_BUFFER)) > 0) {
		this->fileBuffer_->append(buffer, bytesRead);
		totalBytesRead += bytesRead;
	}
	close(fileFd);
	if (bytesRead < 0) {
		(this->resp_ = HttpResponse(400)).sendResp(this->socket_);
		throw std::runtime_error("Error reading file");
	}
}

//TODO: Refactor hanlde()
//TODO: Inlude max client body size
/**
 * handle: Handle the client request
 * @attention Nasty code! Needs refactor
 */
void ClientHandler::handle() {
	this->debug("handling request from ") << this->clientIp_ << std::endl;
	if (!this->fetched_)
		this->fetch();
	if (!this->req_.isValid()) {
		(this->resp_ = HttpResponse(400)).sendResp(this->socket_);
		throw std::runtime_error("Invalid HttpRequest");
	}
	this->debug("Request:") << std::endl << C_ORANGE << this->headers_->data() << C_RESET << std::endl;

	int fileFd = -1;
	std::string fileName = this->server_.getConfig().getRoutes()[0].getRoot() + req_.getUrl();
	if ((fileFd = open(fileName.c_str(), O_RDONLY)) > 0) {
		try {
			buildBody_(fileFd);
		} catch (const std::exception& e) {
			(this->resp_ = HttpResponse(400)).sendResp(this->socket_);
			throw;
		}
		(this->resp_ = HttpResponse(200, this->fileBuffer_->data(), this->fileBuffer_->size(), req_.getUrl())).sendResp(this->socket_);
	} else {
		(this->resp_ = HttpResponse(404)).sendResp(this->socket_);
		throw std::runtime_error(fileName + " not found");
	}
}

/**
 * getSocket: Get client socket
 * @return `int` fd socket reference
 */
int ClientHandler::getSocket() const {
	return this->socket_;
}

const HttpRequest& ClientHandler::fetch() {
	if (this->fetched_)
		return this->req_;
	try {
		char client_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(this->addr_.sin_addr), client_ip, INET_ADDRSTRLEN);
		this->clientIp_ = client_ip;
		loadHeaders_();
		this->fetched_ = true;
	} catch(const std::exception& e) {
		(this->resp_ = HttpResponse(400)).sendResp(this->socket_);
		throw;
	}
	this->req_ = HttpRequest(this->headers_->data());
	return this->req_;
}

const HttpResponse& ClientHandler::getResponse() const {
	return this->resp_;
}

const std::string& ClientHandler::getClientIp() const {
	return this->clientIp_;
}