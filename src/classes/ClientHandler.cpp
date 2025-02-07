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
/**
 * handle: Handle the client request
 * @attention Nasty code! Needs refactor
 */
void ClientHandler::handle() {
	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(this->addr_.sin_addr), client_ip, INET_ADDRSTRLEN); // TODO: May need to replace way of getting ip
	this->debug("handling request from ") << client_ip << std::endl;
	size_t buffer_size = this->server_.getConfig().getClientBodyLimit();
	char *buffer = new char[buffer_size + 1];
	ssize_t bytes = read(this->socket_, buffer, buffer_size);
	if (bytes > 0) {
		buffer[bytes] = 0;
		this->debug("header:") << std::endl << C_YELLOW << buffer << C_RESET << std::endl;
	}
	else
		this->debug("no data to read") << std::endl;
	std::string res = "HTTP/1.1 ";
	std::string html_file = this->server_.getRouteConfig()[0].getRoot() + "/index.html";
	int html_file_fd = open(html_file.c_str(), O_RDONLY);
	if (html_file_fd <= 0) {
		this->warning("Couldn't find " + html_file) << std::endl;
		std::string content = "Couldn't find " + html_file;
		res += "404 Not Found\nContent-Type: text/plain\nContent-Length:" + Convert::ToString(content.length()) + "\n\n" + content;
	} else {
		// test
		HttpRequest req(buffer);
		char buffer_file[2048] = {0};
		ssize_t file_bytes = read(html_file_fd, buffer_file, 2048);
		res += "200 OK\nContent-Type: text/html\nContent-Length:" + Convert::ToString(file_bytes) + "\n\n" + buffer_file;
	}
	write(this->socket_, res.c_str(), res.length());
	delete[] buffer;
	delete this;
}

/**
 * getSocket: Get client socket
 * @return `int` fd socket reference
 */
int ClientHandler::getSocket() const {
	return this->socket_;
}