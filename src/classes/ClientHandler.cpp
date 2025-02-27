#include "./ClientHandler.hpp"
#include "ServerManager.hpp"
#include <cstring>
#include <stdexcept>
#include <string>

std::ostream& ClientHandler::fatal(const std::string& msg) { return Logger::fatal(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg); }
std::ostream& ClientHandler::error(const std::string& msg) { return Logger::error(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg); }
std::ostream& ClientHandler::warning(const std::string& msg) { return Logger::warning(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg); }
std::ostream& ClientHandler::info(const std::string& msg) { return Logger::info(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg); }
std::ostream& ClientHandler::debug(const std::string& msg) { return Logger::debug(C_BLUE + server_.getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg); }

static pollfd createPollfd(int fd) {
	pollfd out;
	out.fd = fd;
	out.events = POLLIN | POLLHUP;
	out.revents = 0;
	return out;
}

ClientHandler::ClientHandler(Runtime& runtime, ServerManager& server, int socket_fd, sockaddr_in addr, socklen_t addrlen):	
	socket_fd_(socket_fd),
	runtime_(runtime),
	server_(server),
	address_(addr, addrlen),
	flags_(READING) {
		struct timeval tv;
		gettimeofday(&tv, 0);
		this->last_alive_ = tv.tv_sec * 1000 + tv.tv_usec / 1000;
		this->runtime_.getSockets().push_back(createPollfd(this->socket_fd_));
		#if LOGGER_DEBUG
			this->debug("New socket") << std::endl;
		#endif
}

ClientHandler::ClientHandler(const ClientHandler& copy):
	socket_fd_(-1),	
	runtime_(copy.runtime_),
	server_(copy.server_),
	flags_(copy.flags_),
	last_alive_(copy.last_alive_) {
		Logger::fatal("A client was created by copy. Client constructors by copy aren't inteeded; the class init and deconstructor interacts with runtime!") << std::endl;
}

ClientHandler& ClientHandler::operator=(const ClientHandler& assign) {
	if (this == &assign)
		return *this;
	Logger::fatal("A client was assigned (operator=). Client assignments aren't inteeded; the class init and deconstructor interacts with runtime!") << std::endl;
	return *this;
}

ClientHandler::~ClientHandler() {
	#if LOGGER_DEBUG
		this->debug("Client request deconstructor") << std::endl;
	#endif
	close(this->socket_fd_);
	this->runtime_.getClients().erase(this->socket_fd_);
	if (this->buffer_.externalBody.is_open()) {
		this->buffer_.externalBody.close();
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
	#if LOGGER_DEBUG
		this->debug("sending header") << std::endl;
	#endif
	std::string header;
	std::ostringstream oss;
	oss << this->getResponse().str();

	if (this->buffer_.externalBody) {
		this->flags_ |= SENDING;
	} else {
		this->flags_ |= SENT;
	}
	header = oss.str();
	if (send(this->socket_fd_, header.data(), header.size(), 0) < 0)
		throw std::runtime_error(EXC_SEND_ERROR);
}

void ClientHandler::sendpayload_() {
	#if LOGGER_DEBUG
		std::ostream& stream = this->debug("sending payload ");
		if (!this->getRequest().getReqLine().empty())
			stream << this->getRequest().getUrl();
		stream << std::endl;
	#endif
	if (!this->buffer_.internalBody.empty()) {
		if (send(this->socket_fd_, this->buffer_.internalBody.c_str(), this->buffer_.internalBody.length(), 0) < 0)
			throw std::runtime_error(EXC_SEND_ERROR);
		this->buffer_.internalBody.clear();
		this->flags_ |= SENT;
	} else if (this->buffer_.externalBody.is_open()) {
		std::ifstream& file = this->buffer_.externalBody;
		char buffer[DF_MAX_BUFFER] = {0};
		if (file.read(buffer, DF_MAX_BUFFER) || file.gcount() > 0) {
			if (send(this->socket_fd_, buffer, file.gcount(), 0) < 0)
				throw std::runtime_error(EXC_SEND_ERROR);
		}
		if (!file) {
			file.close();
			this->flags_ |= SENT;
		}
		#if LOGGER_DEBUG
			if (this->request_.getUrl().find(".html") != std::string::npos) {
				this->debug("sended: ") << buffer << std::endl;
			}
		#endif
	}
}

void ClientHandler::sendResponse() {
	#if LOGGER_DEBUG
		this->debug("sending response") << std::endl;
	#endif
	if (this->flags_ & SENT) return;
	if (!(this->flags_ & SENDING)) {
		if (!(this->flags_ & RESPONSE))
			this->buildResponse(HttpResponse(this->request_));
		this->sendHeader_();
	}
	if (this->buffer_.externalBody || !this->buffer_.internalBody.empty())
		sendpayload_();
	return;
}

const HttpRequest& ClientHandler::buildRequest() {
	if (this->flags_ & FETCHED)
		return this->request_;
	this->flags_ &= ~READING;
	this->flags_ |= FETCHED;
	this->request_ = HttpRequest(this->buffer_.requestBuffer, &this->buffer_.bodyBuffer);
	return this->request_;
}

const HttpRequest& ClientHandler::getRequest() const { return this->request_; }

const HttpResponse& ClientHandler::buildResponse(HttpResponse response) {
	std::string rootFile;
	const RouteConfig *matchingRoot = 0;
	// Open file or build 301 permanent redirection or 302 non-permanent redirection
	if (response.getUrl()) {
		const std::vector<RouteConfig>& routes = this->server_.getRouteConfig();
		for (std::vector<RouteConfig>::const_iterator route = routes.begin(); route != routes.end(); route++) {
			const std::string& locationRoot = route->getPath();
			if (response.getUrl()->size() >= locationRoot.size() && response.getUrl()->substr(0, locationRoot.size()) == locationRoot)
				if (!matchingRoot || locationRoot.size() > matchingRoot->getPath().size())
					matchingRoot = &*route;
		}
		if (!matchingRoot) throw std::runtime_error(EXC_NO_ROUTE);
		if (matchingRoot->getPath() != "/" && matchingRoot->getPath() == this->request_.getUrl())
			return this->buildResponse(HttpResponse(this->request_, *matchingRoot));
		rootFile = matchingRoot->getLocationRoot() + "/" + this->request_.getUrl();
		if (rootFile.at(rootFile.size() - 1) != '/') {
			struct stat s;
			if (!stat(rootFile.c_str(), &s) && s.st_mode & S_IFDIR) {
				if (access((rootFile + "/" + matchingRoot->getIndex()).c_str(), O_RDONLY ) == 0 )
					rootFile.append("/" + matchingRoot->getIndex()) ;
				else
					rootFile.append("/");
			}
		}
		else {
			if (access((rootFile + matchingRoot->getIndex()).c_str(), O_RDONLY) == 0)
				rootFile.append(matchingRoot->getIndex());
		}
		if (this->buffer_.externalBody.is_open())
			this->buffer_.externalBody.close();
		if (rootFile.at(rootFile.size() - 1) != '/')
			this->buffer_.externalBody.open(rootFile.c_str(), std::ios::binary);
		else if (matchingRoot && matchingRoot->isDirectoryListingEnabled()) {
			// Build directory listing
			this->buffer_.internalBody = ListingBuilder::buildBody(request_.getUrl(), rootFile);
		}
		
	}
	std::ifstream& externalBody = this->buffer_.externalBody;

	// Build 404 - Not Found
	if (response.getUrl() && request_.getMethod() == "GET" && (!externalBody.good() || !externalBody.is_open()) && this->buffer_.internalBody.empty() && response.getStatus() != 404) {
		if (externalBody.is_open())
			externalBody.close();
		return this->buildResponse(HttpResponse(this->getRequest(), 404));
	}

	// Build 405 - Method Not Allowed
	if (matchingRoot && !matchingRoot->getMethods().empty()) {
		const std::vector<std::string>::const_iterator method = std::find(matchingRoot->getMethods().begin(), matchingRoot->getMethods().end(), this->request_.getMethod());
		if (method == matchingRoot->getMethods().end())
			return this->buildResponse(HttpResponse(this->request_, 405));
	}

	// Check file for http code
	if (response.getStatus() < 200 || response.getStatus() > 299 ) {
		const std::map<int, std::string>& errorPages = this->server_.getConfig().getErrorPages();
		int status = response.getStatus();
		if (errorPages.find(status) != errorPages.end()) {
			if (externalBody.is_open()) {
				this->fatal("error on reading file: '" + rootFile + "': ") << strerror(errno) << std::endl;
				externalBody.close();
			}
				externalBody.open(errorPages.at(status).c_str(), std::ios::binary);
			if (externalBody.is_open()) {
				if (!externalBody.good())
					externalBody.close();
				else
					response.getHeaders()[H_CONTENT_TYPE] = HttpResponse::getType(errorPages.at(status));
			}
		}
	} else {
		if (this->request_.getMethod() == "DELETE") {
			if (this->buffer_.externalBody.is_open())
				this->buffer_.externalBody.close();
			if (unlink(rootFile.c_str()) < 0)
				return this->buildResponse(HttpResponse(this->request_, 404));
			response.setStatus(204);
		} else if (this->request_.getMethod() == "POST" && matchingRoot->getCgi().first.empty()) {
			try{
				request_.buildBody(matchingRoot->getFinalPath(), matchingRoot->getUploadPath());
				response.setStatus(201);
			}
			catch (const std::exception &e) {
				this->error("POST: ") << e.what() << std::endl;
				return buildResponse(HttpResponse(request_, 500));
			}
		}
	}

	if (matchingRoot && this->buffer_.internalBody.empty() && !matchingRoot->getCgi().first.empty()) {
		try {
			CgiHandler	cgi ( this, matchingRoot ) ;
			if (cgi.isValidCgi()) {
				cgi.run() ; 
				this->buffer_.internalBody = cgi.getOutputBody();
				response.getHeaders()[H_CONTENT_LENGTH] = Convert::ToString(this->buffer_.internalBody.size()) ;
				response.getHeaders()[H_CONTENT_TYPE] = cgi.getOutputHeaders().at(H_CONTENT_TYPE) ;
			}
		}
		catch(const std::exception& e) {
			Logger::error("CGI Error: ") << e.what() << std::endl;
			return buildResponse(HttpResponse(this->request_, 500));
		}
	}

	// Final build (may need some modifications if building internal html)
	if (this->buffer_.internalBody.empty() && externalBody.is_open()) {
		externalBody.seekg(0, std::ios::end);
		response.getHeaders()[H_CONTENT_LENGTH] = Convert::ToString(externalBody.tellg());
		externalBody.seekg(0, std::ios::beg);
		if (response.getHeaders().find(H_CONTENT_TYPE) == response.getHeaders().end())
			response.getHeaders()[H_CONTENT_TYPE] = HttpResponse::getType(rootFile);
	}
	else if (!this->buffer_.internalBody.empty()) {
		response.getHeaders()[H_CONTENT_TYPE] = "text/html";
		response.getHeaders()[H_CONTENT_LENGTH] = Convert::ToString(this->buffer_.internalBody.size());
	}
	else
		response.getHeaders()[H_CONTENT_LENGTH] = "0";
	this->response_ = response;
	this->flags_ |= RESPONSE;
	return this->response_;
}

void ClientHandler::flush() {
	if (this->buffer_.externalBody.is_open()) {
		this->buffer_.externalBody.close();
	}
	if (!this->buffer_.internalBody.empty())
		this->buffer_.internalBody.clear();
	if (this->buffer_.requestBuffer) {
		delete this->buffer_.requestBuffer;
		this->buffer_.requestBuffer = 0;
	}
	buffer_.bodyBuffer.clear();
	buffer_.boundary.clear();
	buffer_.bodyReading = false;
	this->request_ = HttpRequest();
	this->response_ = HttpResponse();
	this->flags_ = READING;
}

int ClientHandler::parseBodyInfo(std::string *request, bool bodyLen){
	if (!request || request->empty())
		throw (std::runtime_error("Empty request"));
	
	std::stringstream ss(*request);
	std::string line;

	while (std::getline(ss, line))
	{
		line = line.substr(0, line.size() - 1);
		if (line.empty())
			break;
		size_t sep = line.find(':', 0);
		if (sep != line.npos){
			std::string key = line.substr(0, sep);
			std::string value = line.substr(sep + 2, line.size() - sep - 2);
			if (bodyLen && key == "Content-Length")
				return Convert::ToInt(value);
			if (key == "Content-Type" && value.find("multipart/form-data") != value.npos)
				return true;
		}
	}
	return false;
}

void ClientHandler::readSocket(){
	char buffer[DF_MAX_BUFFER + 1];
	ssize_t bytesRead = 0;

	bzero(buffer, DF_MAX_BUFFER + 1);
	if (!this->buffer_.requestBuffer)
		this->buffer_.requestBuffer = new std::string("");
	if ((bytesRead = recv(this->socket_fd_, buffer, DF_MAX_BUFFER, 0)) > 0){
		if (buffer_.bodyReading)
			this->buffer_.bodyBuffer.append(buffer, bytesRead);
		else if (!buffer_.bodyReading && strnstr(buffer, "\r\n\r\n", bytesRead)){
			char *tmp = strnstr(buffer, "\r\n\r\n", bytesRead);
			buffer_.requestBuffer->append(buffer, tmp - buffer);
			if (parseBodyInfo(buffer_.requestBuffer, false)){
				if ((unsigned long long)parseBodyInfo(buffer_.requestBuffer, true) > getServerConfig().getClientBodyLimit())
					throw std::runtime_error("Content-length exceed thw Body limit");
				buffer_.bodyBuffer.append(buffer + (tmp - buffer + 4), bytesRead - (tmp - buffer - 4));
				buffer_.bodyReading = true;
			}
			else{
				this->flags_ &= ~READING;
				return ;
			}
		}
		else
			this->buffer_.requestBuffer->append(buffer, bytesRead);
		if (buffer_.bodyBuffer.find(buffer_.boundary + "--") != std::string::npos){
			this->flags_ &= ~READING;
			buffer_.bodyReading = false;
			return ;
		}
	}
	else if (bytesRead < 0) {
		this->flags_ |= FETCHED;
		throw std::runtime_error(EXC_SOCKET_READ);
	}
	else { this->buildRequest(); }
}

HttpResponse& ClientHandler::getResponse() { return this->response_; }
const char *ClientHandler::getClientIp() const { return this->address_.clientIp; }

int8_t ClientHandler::getFlags() const { return this->flags_; }
void ClientHandler::clearFlag(int8_t flag) { this->flags_ &= ~flag; }
void ClientHandler::setFlag(int8_t flag) { this->flags_ |= flag; }
const ServerConfig& ClientHandler::getServerConfig() const { return this->server_.getConfig(); }
int ClientHandler::getFd() const { return this->socket_fd_; }
unsigned long long ClientHandler::getLastAlive() const { return this->last_alive_; }
void ClientHandler::updateLastAlive() {
	struct timeval tv;
	gettimeofday(&tv, 0);
	this->last_alive_ = tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
