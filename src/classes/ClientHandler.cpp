#include "./ClientHandler.hpp"
#include "ServerManager.hpp"
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>

std::ostream& ClientHandler::fatal(const std::string& msg) const {
	if (this->server_) return Logger::fatal(C_BLUE + server_->getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg);
	else return Logger::fatal("ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg);
}
std::ostream& ClientHandler::error(const std::string& msg) const {
	if (this->server_) return Logger::error(C_BLUE + server_->getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg);
	else return Logger::error("ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg);
}
std::ostream& ClientHandler::warning(const std::string& msg) const {;
	if (this->server_) return Logger::warning(C_BLUE + server_->getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg);
	else return Logger::warning("ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg);
}
std::ostream& ClientHandler::info(const std::string& msg) const {;
	if (this->server_) return Logger::info(C_BLUE + server_->getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg);
	else return Logger::info("ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg);
}
std::ostream& ClientHandler::debug(const std::string& msg) const {
	if (this->server_) return Logger::debug(C_BLUE + server_->getConfig().getServerNames()[0] + C_RESET + ": ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg);
	else return Logger::debug("ClientHandler (fd: " + Convert::ToString(this->socket_fd_) + "): " + msg);
}

static pollfd createPollfd(int fd) {
	pollfd out;
	out.fd = fd;
	out.events = POLLIN | POLLHUP;
	out.revents = 0;
	return out;
}

ClientHandler::ClientHandler(Runtime& runtime, const ServerManager& origin, int socket_fd, sockaddr_in addr, socklen_t addrlen):	
	socket_fd_(socket_fd),
	runtime_(runtime),
	hostServer_(origin),
	server_(0),
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
	hostServer_(copy.hostServer_),
	server_(0),
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
	if (this->request_.getAllBody())
		delete this->request_.getAllBody() ;
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

	if (this->buffer_.externalBody.is_open() || !this->buffer_.internalBody.empty()) {
		this->flags_ |= SENDING;
	} else {
		this->flags_ |= SENT;
	}
	header = oss.str();
	ssize_t bytesSent = 0;
	if ((bytesSent = send(this->socket_fd_, header.data(), header.size(), 0)) < 0)
		throw std::runtime_error(EXC_SEND_ERROR);
	else if (bytesSent || !bytesSent)
		return;
}

void ClientHandler::sendpayload_() {
	#if LOGGER_DEBUG
		this->debug("sending payload ") << std::endl;
	#endif
	ssize_t bytesSent = 0;
	if (!this->buffer_.internalBody.empty()) {
		if ((bytesSent = send(this->socket_fd_, this->buffer_.internalBody.c_str(), this->buffer_.internalBody.length(), 0)) < 0)
			throw std::runtime_error(EXC_SEND_ERROR);
		else if (bytesSent || !bytesSent) {
			this->buffer_.internalBody.clear();
			this->flags_ |= SENT;
		}
	} else {
		std::ifstream& file = this->buffer_.externalBody;
		char buffer[DF_MAX_BUFFER + 1] = {0};
		if (file.read(buffer, DF_MAX_BUFFER) || file.gcount() > 0) {
			if ((bytesSent = send(this->socket_fd_, buffer, file.gcount(), 0)) < 0)
				throw std::runtime_error(EXC_SEND_ERROR);
			else if (bytesSent || !bytesSent) (void)bytesSent;
		}
		if (!file) {
			file.close();
			this->flags_ |= SENT;
		}
	}
}

void ClientHandler::sendResponse() {
	if (this->flags_ & SENT) return;
	if (!(this->flags_ & SENDING)) {
		if (!(this->flags_ & RESPONSE))
			this->buildResponse(HttpResponse(this->request_));
		this->sendHeader_();
	}
	else if (this->buffer_.externalBody.is_open() || !this->buffer_.internalBody.empty())
		sendpayload_();
	return;
}

const HttpRequest& ClientHandler::buildRequest() {
	this->request_ = HttpRequest(this->buffer_.requestBuffer, &this->buffer_.bodyBuffer);
	return this->request_;
}

const HttpRequest& ClientHandler::getRequest() const { return this->request_; }

const HttpResponse& ClientHandler::buildResponse(HttpResponse response) {
	std::string rootFile;
	const RouteConfig *matchingRoot = 0;
	// Open file or build 301 permanent redirection or 302 non-permanent redirection
	if (response.getUrl()) {
		const std::vector<RouteConfig>& routes = this->server_->getRouteConfig();
		for (std::vector<RouteConfig>::const_iterator route = routes.begin(); route != routes.end(); route++) {
			const std::string& locationRoot = route->getPath();
			if (response.getUrl()->size() >= locationRoot.size() && response.getUrl()->substr(0, locationRoot.size()) == locationRoot)
				if (!matchingRoot || locationRoot.size() > matchingRoot->getPath().size())
					matchingRoot = &*route;
		}
		if (!matchingRoot) throw std::runtime_error(EXC_NO_ROUTE);
		if (matchingRoot->getPath() != "/"
			&& (matchingRoot->getPath() == this->request_.getUrl() || !matchingRoot->getReturn().empty()))
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
			try {
				this->buffer_.internalBody = ListingBuilder::buildBody(request_.getUrl(), rootFile);
			} catch (const std::exception& e) {
				std::string msg(e.what());
				if (msg == EXC_NO_DIR)
					return this->buildResponse(HttpResponse(this->request_, 404));
				return this->buildResponse(HttpResponse(this->request_, 500));
			}
		}
		
	}

	// Build 404 - Not Found
	if (response.getUrl() && (request_.getMethod() == "GET" || request_.getMethod() == "POST") && (!this->buffer_.externalBody.good() || !this->buffer_.externalBody.is_open()) && this->buffer_.internalBody.empty() && response.getStatus() != 404) {
		if (this->buffer_.externalBody.is_open())
			this->buffer_.externalBody.close();
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
		if (this->buffer_.externalBody.is_open())
			this->buffer_.externalBody.close();
		const std::map<int, std::string>& errorPages = this->server_->getConfig().getErrorPages();
		int status = response.getStatus();
		if (errorPages.find(status) != errorPages.end())
			this->buffer_.externalBody.open(errorPages.at(status).c_str(), std::ios::binary);
		if (!this->buffer_.externalBody.is_open() || this->buffer_.externalBody.fail())
			this->buffer_.internalBody = ErrorBuilder::buildBody(response);
		else
			response.getHeaders()[H_CONTENT_TYPE] = HttpResponse::getType(errorPages.at(status));
	} else {
		if (this->request_.getMethod() == "DELETE") {
			if (this->buffer_.externalBody.is_open())
				this->buffer_.externalBody.close();
			if (unlink(rootFile.c_str()) < 0)
				return this->buildResponse(HttpResponse(this->request_, 404));
			response.setStatus(204);
		} else if (this->request_.getMethod() == "POST" && matchingRoot->getCgi().first.empty()) {
			if (matchingRoot->isUploadAccepted()) {
				if (this->request_.getBoundary().empty())
					return this->buildResponse(HttpResponse(this->request_, 415));
				try{
					request_.buildBody(matchingRoot->getFinalPath(), matchingRoot->getUploadPath());
					response.setStatus(201);
				}
				catch (const std::exception &e) {
					this->error("POST: ") << e.what() << std::endl;
					return buildResponse(HttpResponse(request_, 500));
				}
			} else
				return this->buildResponse(HttpResponse(this->request_, 403));
		}
	}

	if (matchingRoot && this->buffer_.internalBody.empty() && !matchingRoot->getCgi().first.empty()) {
		try {
			CgiHandler	cgi ( this, matchingRoot ) ;
			if (cgi.isValidCgi()) {
				if (this->buffer_.externalBody.is_open())
					this->buffer_.externalBody.close();
				cgi.run() ; 
				this->buffer_.internalBody = cgi.getOutputBody(); 
				for (std::map<std::string, std::string>::const_iterator it = cgi.getOutputHeaders().begin() ;
					it != cgi.getOutputHeaders().end() ; it++ ) {
						response.getHeaders()[it->first] = it->second ;
					}
			}
		}
		catch(const std::exception& e) {
			Logger::error("CGI Error: ") << e.what() << std::endl;
			return buildResponse(HttpResponse(this->request_, 500));
		}
	}

	// Final build
	if (this->buffer_.internalBody.empty() && this->buffer_.externalBody.is_open()) {
		this->buffer_.externalBody.seekg(0, std::ios::end);
		response.getHeaders()[H_CONTENT_LENGTH] = Convert::ToString(this->buffer_.externalBody.tellg());
		this->buffer_.externalBody.seekg(0, std::ios::beg);
		if (response.getHeaders().find(H_CONTENT_TYPE) == response.getHeaders().end())
			response.getHeaders()[H_CONTENT_TYPE] = HttpResponse::getType(rootFile);
	}
	else if (!this->buffer_.internalBody.empty()) {
		if (response.getHeaders().find(H_CONTENT_TYPE) == response.getHeaders().end())
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
	this->server_ = 0;
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
	buffer_.boundaryEnd.clear();
	buffer_.bodyReading = false;
	this->buffer_.bodySize = 0;
	this->buffer_.bodyWantedSize = 0;
	this->request_ = HttpRequest();
	this->response_ = HttpResponse();
	this->flags_ = READING;
}

unsigned long long ClientHandler::parseBodyInfo(std::string *request, bool bodyLen){
	if (!request || request->empty())
		throw (std::runtime_error("Empty request"));
	
	std::stringstream ss(*request);
	std::string line;

	while (std::getline(ss, line))
	{
		if (line.at(line.size() - 1) == '\r')
			line = line.substr(0, line.size() - 1);
		if (line.empty())
			break;
		size_t sep = line.find(':', 0);
		if (sep != line.npos){
			std::string key = line.substr(0, sep);
			std::string value = line.substr(sep + 2);
			if (bodyLen && key == "Content-Length") {
				if (value.empty())
					return 0;
				return Convert::ToT<unsigned long long, const std::string>(value);
			}
			if (!bodyLen && buffer_.boundary.empty() && key == "Content-Type") {
				if (value.find("multipart/form-data") != value.npos) {
					buffer_.boundary = "--" + value.substr(value.find("boundary=")+9, value.size());
					buffer_.boundaryEnd = buffer_.boundary + "--";
				}
				return true;
			}
		}
	}
	return false;
}

static void handleSignals(const char *buffer, ssize_t bytesRead) {
	switch (bytesRead) {
		case 1:
			if (static_cast<int>(*buffer) == 4) throw std::runtime_error(EXC_CLOSE);
			break ;
		case 5: {
			char	arr[5] = {-1, -12, -1, -3, 6} ;
			if (memmem(buffer, bytesRead, arr, 5)) throw std::runtime_error(EXC_CLOSE);
			break ;
		}
	}
}

void ClientHandler::readSocket(){
	char buffer[DF_MAX_BUFFER + 1];
	ssize_t bytesRead = 0;
	size_t cursor = 0;
	char *buffer_cursor = 0;

	bzero(buffer, DF_MAX_BUFFER + 1);
	if (!this->buffer_.requestBuffer)
		this->buffer_.requestBuffer = new std::string("");

	if ((bytesRead = recv(this->socket_fd_, buffer, DF_MAX_BUFFER, 0)) > 0)
	{
		handleSignals(buffer, bytesRead);
		if (!buffer_.bodyReading){
			this->buffer_.requestBuffer->append(buffer, bytesRead);
			buffer_cursor = static_cast<char*>(memmem(const_cast<char *>(buffer_.requestBuffer->c_str()), buffer_.requestBuffer->size(), "\r\n\r\n", 4));
		}
		if (this->flags_ & THROWING) {this->buffer_.bodySize += bytesRead;}
		else if (buffer_.bodyReading)
		{
			this->buffer_.bodyBuffer.append(buffer, bytesRead);
			this->buffer_.bodySize += this->buffer_.bodyBuffer.size();
			if (this->buffer_.bodyBuffer.size() > this->getHostServer().getMaxBody()){
				this->buffer_.bodyBuffer.clear();
				this->flags_ |= ERR_BODYTOOBIG;
			}
		}
		else if (!buffer_.bodyReading && buffer_cursor)
		{
			cursor = buffer_cursor - buffer_.requestBuffer->c_str() + 4;
			if (parseBodyInfo(buffer_.requestBuffer, false)){
				this->buffer_.bodyWantedSize = parseBodyInfo(buffer_.requestBuffer, true);
				if (this->buffer_.bodyWantedSize > this->getHostServer().getMaxBody())
					this->flags_ |= ERR_BODYTOOBIG;
				else if (!this->buffer_.bodyWantedSize)
					this->flags_ |= ERR_NOLENGTH;
				else {
					buffer_.bodyBuffer = buffer_.requestBuffer->substr(cursor, buffer_.requestBuffer->size() - cursor);
					this->buffer_.bodySize = this->buffer_.bodyBuffer.size();
					std::string *tmp = new std::string(buffer_.requestBuffer->substr(0, cursor - 3));
					delete buffer_.requestBuffer;
					buffer_.requestBuffer = tmp;
					buffer_.bodyReading = true;
				}
				if (this->flags_ & THROWING)
					this->buffer_.bodySize = std::string(buffer, bytesRead).substr(cursor, bytesRead - cursor).size();
			}
			else{ this->flags_ &= ~READING; return ;}
		}
		if ((buffer_.boundaryEnd.empty() && this->buffer_.bodyWantedSize && this->buffer_.bodySize >= this->buffer_.bodyWantedSize)
			|| (!buffer_.boundaryEnd.empty() && memmem(buffer, bytesRead, buffer_.boundaryEnd.c_str(), buffer_.boundaryEnd.size()))){
			this->flags_ &= ~READING;
			buffer_.bodyReading = false;
			return ;
		}
	}
	else if (bytesRead < 0)
		throw std::runtime_error(EXC_SOCKET_READ);
	else
	{
		if (!this->buffer_.requestBuffer || this->buffer_.requestBuffer->empty())
			throw std::runtime_error(EXC_NO_BUFFER);
		this->flags_ &= ~READING;
		return;
	}
}

void ClientHandler::retrieveServer() {
	std::string clientServername;
	size_t cursor = 0;
	const std::string& clientHost = this->request_.getHeaders().at(H_HOST);
	if ((cursor = clientHost.find(":")) != std::string::npos) {
		clientServername = clientHost.substr(0, cursor);
	} else clientServername = clientHost;
	for(std::vector<ServerManager *>::const_iterator it = this->hostServer_.getVirtualHosts().begin();
		it != this->hostServer_.getVirtualHosts().end(); it++) {
			if (std::find((*it)->getConfig().getServerNames().begin(), (*it)->getConfig().getServerNames().end(), clientServername) != (*it)->getConfig().getServerNames().end()) {
				this->server_ = *it;
				break;
			}
		}
	if (!this->server_)
		this->server_ = &this->hostServer_;
}

HttpResponse& ClientHandler::getResponse() { return this->response_; }
const char *ClientHandler::getClientIp() const { return this->address_.clientIp; }

int8_t ClientHandler::getFlags() const { return this->flags_; }
void ClientHandler::clearFlag(int8_t flag) { this->flags_ &= ~flag; }
void ClientHandler::setFlag(int8_t flag) { this->flags_ |= flag; }
const ServerConfig& ClientHandler::getServerConfig() const {
	if (this->server_)
		return this->server_->getConfig();
	else {
		#if LOGGER_DEBUG > 0
			this->warning("webserver getting serverConfig from client, but client has no designated server yet") << std::endl;
		#endif
		return this->hostServer_.getConfig();
	}
}
int ClientHandler::getFd() const { return this->socket_fd_; }
unsigned long long ClientHandler::getLastAlive() const { return this->last_alive_; }
void ClientHandler::updateLastAlive() {
	struct timeval tv;
	gettimeofday(&tv, 0);
	this->last_alive_ = tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
const s_clientBuffer& ClientHandler::getBuffer() const { return this->buffer_; }
bool ClientHandler::hasServer() const { return this->server_; }
const ServerManager& ClientHandler::getHostServer() const { return this->hostServer_; }