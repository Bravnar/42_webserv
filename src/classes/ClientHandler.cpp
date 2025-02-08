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
	len_(len) {
		runtime.getSockets().push_back(makeClientSocket(client_socket));
		this->socket_ = client_socket;
		this->debug("New socket") << std::endl;
}

ClientHandler::ClientHandler(const ClientHandler& copy):
	runtime_(copy.runtime_),
	server_(copy.server_),
	addr_(copy.addr_),
	len_(copy.len_),
	socket_(copy.socket_) {
}

ClientHandler::~ClientHandler() {
	this->debug("Client request deconstructor") << std::endl;
	close(this->socket_);
	{
		std::vector<pollfd>& sockets_ = this->runtime_.getSockets();
		std::vector<pollfd>::iterator it_sockets = sockets_.begin();
		while (it_sockets != sockets_.end()) {
			if (it_sockets->fd == this->socket_) {
				sockets_.erase(it_sockets);
				break;
			}
			it_sockets++;
		}
	}
	{
		std::vector<ClientHandler *>& clients_ = this->runtime_.getClients();
		std::vector<ClientHandler *>::iterator it_clients = clients_.begin();
		while (it_clients != clients_.end()) {
			if (*it_clients == this) {
				it_clients = clients_.erase(it_clients);
				break;
			}
			it_clients++;
		}
	}
}

//TODO: Refactor hanlde()
//TODO: Inlude max client body size
/**
 * handle: Handle the client request
 * @attention Nasty code! Needs refactor
 */
void ClientHandler::handle() {
	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(this->addr_.sin_addr), client_ip, INET_ADDRSTRLEN);
	this->debug("handling request from ") << client_ip << std::endl;

	char buffer[DF_MAX_BUFFER];
	std::string *headers = new std::string("");
	ssize_t bytesRead;
	size_t totalBytesRead = 0;
	while ((bytesRead = recv(this->socket_, buffer, DF_MAX_BUFFER, 0)) > 0) {
		headers->append(buffer, bytesRead);
		totalBytesRead += bytesRead;
		if (bytesRead < DF_MAX_BUFFER)
			break;
	}
	if (bytesRead < 0) {
		this->fatal("Error reading from socket") << std::endl;
		delete headers;
		HttpResponse(400).sendResp(this->socket_);
		delete this;
		return;
	}
	this->debug("Request:") << std::endl << C_ORANGE << headers->data() << C_RESET << std::endl;
	HttpRequest req(headers->data());
	delete headers;

	int fileFd = -1;
	std::string fileName = this->server_.getConfig().getRoutes()[0].getRoot() + req.getUrl();
	if (req.isValid() && (fileFd = open(fileName.c_str(), O_RDONLY)) > 0) {
		std::string *content = new std::string("");

		bytesRead = 0;
		totalBytesRead = 0;
		while ((bytesRead = read(fileFd, buffer, DF_MAX_BUFFER)) > 0) {
			content->append(buffer, bytesRead);
			totalBytesRead += bytesRead;
		}
		close(fileFd);
		if (bytesRead < 0) {
			this->fatal("Error reading file") << std::endl;
			delete content;
			HttpResponse(400).sendResp(this->socket_);
			delete this;
			return;
		}
		HttpResponse(200, content->data(), content->size(), req.getUrl()).sendResp(this->socket_);
		delete content;
	} else if (!req.isValid()){
		Logger::error("Invalid request");
		HttpResponse(400).sendResp(this->socket_);
	} else {
		Logger::error("File not found");
		HttpResponse(404).sendResp(this->socket_);
	}
	
	delete this;
}

/**
 * getSocket: Get client socket
 * @return `int` fd socket reference
 */
int ClientHandler::getSocket() const {
	return this->socket_;
}