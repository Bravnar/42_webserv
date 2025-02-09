#ifndef RUNTIME_HPP
# define RUNTIME_HPP

# include "./ClientHandler.hpp"
# include "./ServerManager.hpp"

class ClientHandler;

class Runtime {
	private:
		std::ostream& fatal(const std::string&);
		std::ostream& error(const std::string&);
		std::ostream& warning(const std::string&);
		std::ostream& info(const std::string&);
		std::ostream& debug(const std::string&);
		void checkServers_();
		void checkClients_();
		std::map<int, ServerManager *> servers_;
		std::vector<ClientHandler *> clients_;
		std::vector<pollfd> sockets_;
		int syncPipe_[2];
		pollfd syncPoll_;
		bool isSyncing_;
	public:
		//TODO: Canonical

		Runtime(const std::vector<ServerConfig>&);
		Runtime(const Runtime&);
		Runtime& operator=(const Runtime&);
		~Runtime();

		// Member functions

		void runServers();
		void closeServers();
		std::vector<pollfd>& getSockets();
		std::vector<ClientHandler *>& getClients();
		void Sync();
		pollfd *getSocket(int socket_fd_);
};

#endif