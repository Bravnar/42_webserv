#include "./ClientHandler.hpp"

std::ostream& ClientHandler::fatal(const std::string& msg) { return Logger::fatal(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg); }
std::ostream& ClientHandler::error(const std::string& msg) { return Logger::error(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg); }
std::ostream& ClientHandler::warning(const std::string& msg) { return Logger::warning(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg); }
std::ostream& ClientHandler::info(const std::string& msg) { return Logger::info(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg); }
std::ostream& ClientHandler::debug(const std::string& msg) { return Logger::debug(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg); }

static pollfd createPollfd(int fd) {
	pollfd out;
	out.fd = fd;
	out.events = POLLIN;
	out.revents = 0;
	return out;
}

ClientHandler::ClientHandler(Runtime& runtime, ServerManager& server, int socket_fd, sockaddr_in addr, socklen_t addrlen):	
	socket_fd_(socket_fd),
	runtime_(runtime),
	server_(server),
	address_(addr, addrlen),
	flags_(0) {
		this->runtime_.getSockets().push_back(createPollfd(this->socket_fd_));
		#if LOGGER_DEBUG > 0
			this->debug("New socket") << std::endl;
		#endif
}

ClientHandler::ClientHandler(const ClientHandler& copy):
	socket_fd_(-1),	
	runtime_(copy.runtime_),
	server_(copy.server_),
	flags_(copy.flags_) {
		Logger::fatal("A client was created by copy. Client constructors by copy aren't inteeded; the class init and deconstructor interacts with runtime!") << std::endl;
}

ClientHandler& ClientHandler::operator=(const ClientHandler& assign) {
	if (this == &assign)
		return *this;
	Logger::fatal("A client was assigned (operator=). Client assignments aren't inteeded; the class init and deconstructor interacts with runtime!") << std::endl;
	return *this;
}

ClientHandler::~ClientHandler() {
	#if LOGGER_DEBUG > 0
		this->debug("Client request deconstructor") << std::endl;
	#endif
	close(this->socket_fd_);
	this->runtime_.getClients().erase(this->socket_fd_);
	if (this->buffer_.fileStream.is_open()) {
		this->buffer_.fileStream.close();
	}
	delete this->buffer_.requestBuffer;
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
}

void ClientHandler::sendHeader_() {
	#if LOGGER_DEBUG > 0
		this->debug("sending header") << std::endl;
	#endif
	std::string header;
	std::ostringstream oss;
	oss << this->getResponse().str();

	if (this->buffer_.fileStream) {
		this->flags_ |= SENDING;
	} else {
		this->flags_ |= SENT;
	}
	header = oss.str();
	if (send(this->socket_fd_, header.data(), header.size(), 0) < 0) {
		throw std::runtime_error(EXC_SEND_ERROR);
	}
	#if LOGGER_DEBUG > 0
		if (this->request_.getUrl().find_first_of(".html") != std::string::npos)
			this->debug("sended: ") << std::endl << header << std::endl;
	#endif
}

void ClientHandler::sendPlayload_() {
	#if LOGGER_DEBUG > 0
		std::ostream& stream = this->debug("sending playload ");
		if (!this->getRequest().getReqLine().empty())
			stream << this->getRequest().getUrl();
		stream << std::endl;
	#endif
	std::ifstream& file = this->buffer_.fileStream;
	char buffer[DF_MAX_BUFFER] = {0};
	if (file.read(buffer, DF_MAX_BUFFER) || file.gcount() > 0) {
		if (send(this->socket_fd_, buffer, file.gcount(), 0) < 0) {
			throw std::runtime_error(EXC_SEND_ERROR);
		}
	}
	if (!file) {
		file.close();
		this->flags_ |= SENT;
	}
	#if LOGGER_DEBUG > 0
		if (this->request_.getUrl().find(".html") != std::string::npos) {
			this->debug("sended: ") << buffer << std::endl;
		}
	#endif
}

void ClientHandler::sendResponse() {
	#if LOGGER_DEBUG > 0
		this->debug("sending response") << std::endl;
	#endif
	if (this->flags_ & SENT) return;
	if (!(this->flags_ & SENDING)) {
		if (!(this->flags_ & RESPONSE))
			this->buildResponse(HttpResponse(this->request_));
		this->sendHeader_();
	}
	if (this->buffer_.fileStream)
		sendPlayload_();
	return;
}

const HttpRequest& ClientHandler::buildRequest() {
	if (this->flags_ & FETCHED)
		return this->request_;
	this->flags_ &= ~READING;
	this->flags_ |= FETCHED;
	#if LOGGER_DEBUG > 0
		this->debug("Request: ") << std::endl << C_ORANGE << this->buffer_.requestBuffer->data() << C_RESET << std::endl;
	#endif
	this->request_ = HttpRequest(this->buffer_.requestBuffer);
	return this->request_;
}

const HttpRequest& ClientHandler::getRequest() const { return this->request_; }

const HttpResponse& ClientHandler::buildResponse(HttpResponse response) {
	// -> CGI

	// Open file
	std::string rootFile;
	if (response.getUrl()) {
		const RouteConfig *matchingRoot = 0;
		const std::vector<RouteConfig>& routes = this->server_.getRouteConfig();
		for (std::vector<RouteConfig>::const_iterator route = routes.begin(); route != routes.end(); route++) {
			const std::string& locationRoot = route->getPath();
			if (response.getUrl()->size() >= locationRoot.size() && response.getUrl()->substr(0, locationRoot.size()) == locationRoot)
				if (!matchingRoot || locationRoot.size() > matchingRoot->getPath().size())
					matchingRoot = &*route;
		}
		if (matchingRoot) {
			if (matchingRoot->getPath() != "/" && matchingRoot->getPath() == this->request_.getUrl())
				return this->buildResponse(HttpResponse(this->request_, *matchingRoot)); // 301 redirect constructor
			rootFile = matchingRoot->getLocationRoot() + "/" + this->request_.getUrl();
			if (rootFile.at(rootFile.size() - 1) != '/') {
				struct stat s;
				stat(rootFile.c_str(), &s);
				if (s.st_mode & S_IFDIR) rootFile.append("/" + this->server_.getConfig().getIndex());
			}
			else rootFile.append(this->server_.getConfig().getIndex());
		}
		if (this->buffer_.fileStream.is_open())
			this->buffer_.fileStream.close();
		this->buffer_.fileStream.open(rootFile.c_str(), std::ios::binary);
	}
	std::ifstream& fileStream = this->buffer_.fileStream;

	// Build 404
	if (!rootFile.empty() && !fileStream.good() && response.getStatus() != 404) {
		#if LOGGER_DEBUG > 0
			if (!rootFile.empty())
				Logger::debug(EXC_FILE_NOT_FOUND(rootFile)) << std::endl;
		#endif
		if (this->buffer_.fileStream.is_open())
			this->buffer_.fileStream.close();
		return this->buildResponse(HttpResponse(this->getRequest(), 404));
	}

	// Check file for http code
	if (response.getStatus() < 200 || response.getStatus() > 299 ) {
		const std::map<int, std::string>& errorPages = this->server_.getConfig().getErrorPages();
		int status = response.getStatus();
		if (errorPages.find(status) != errorPages.end()) {
			if (this->buffer_.fileStream.is_open())
				this->buffer_.fileStream.close();
			this->buffer_.fileStream.open(errorPages.at(status).c_str(), std::ios::binary);
			if (!this->buffer_.fileStream.good() && this->buffer_.fileStream.is_open()) {
				this->buffer_.fileStream.close();
			} else {
				response.getHeaders()[H_CONTENT_TYPE] = HttpResponse::getType(errorPages.at(status));
			}
		}
	}

	// Final build (may need some modifications if building internal html)
	if (fileStream.is_open()) {
		fileStream.seekg(0, std::ios::end);
		response.getHeaders()[H_CONTENT_LENGTH] = Convert::ToString(this->buffer_.fileStream.tellg());
		fileStream.seekg(0, std::ios::beg);
		response.getHeaders()[H_CONTENT_TYPE] = HttpResponse::getType(rootFile);
	}
	else
		response.getHeaders()[H_CONTENT_LENGTH] = "0";
	this->response_ = response;
	this->flags_ |= RESPONSE;
	return this->response_;
}

void ClientHandler::flush() {
	if (this->buffer_.fileStream.is_open()) {
		this->buffer_.fileStream.close();
	}
	if (this->buffer_.requestBuffer) {
		delete this->buffer_.requestBuffer;
		this->buffer_.requestBuffer = 0;
	}
	this->request_ = HttpRequest();
	this->response_ = HttpResponse();
	this->flags_ = 0;
}

HttpResponse& ClientHandler::getResponse() { return this->response_; }
const char *ClientHandler::getClientIp() const { return this->address_.clientIp; }

void ClientHandler::readSocket() {
	char buffer[DF_MAX_BUFFER];
	if (!this->buffer_.requestBuffer)
		this->buffer_.requestBuffer = new std::string("");
	ssize_t bytesRead;
	if ((bytesRead = recv(this->socket_fd_, buffer, DF_MAX_BUFFER, 0)) > 0) {
		this->buffer_.requestBuffer->append(buffer, bytesRead);
		#if LOGGER_LEVEL > 0
			this->debug("Syncing") << std::endl;
		#endif
		this->runtime_.Sync();
	}
	else if (bytesRead < 0) {
		this->flags_ |= FETCHED;
		throw std::runtime_error(EXC_SOCKET_READ); }
	else { this->buildRequest(); }
}

int8_t ClientHandler::getFlags() const { return this->flags_; }
void ClientHandler::clearFlag(int8_t flag) { this->flags_ &= ~flag; }
void ClientHandler::setFlag(int8_t flag) { this->flags_ |= flag; }
const ServerManager& ClientHandler::getServer() const { return this->server_; }
int ClientHandler::getFd() const { return this->socket_fd_; }