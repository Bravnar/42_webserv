#ifndef RUNTIME_HPP
# define RUNTIME_HPP

# include "./ClientHandler.hpp"
# include "./ServerManager.hpp"
# include <csignal>

class ClientHandler;

class Runtime {
	private:
		// Forbidden Canonical
		// (A runtime is unique, obviously)

		Runtime(const Runtime&);
		Runtime& operator=(const Runtime&);

		std::ostream& fatal(const std::string&);
		std::ostream& error(const std::string&);
		std::ostream& warning(const std::string&);
		std::ostream& info(const std::string&);
		std::ostream& debug(const std::string&);

		// Properly handle exit
		void handleExit_();
		// Handle polling SyncPipe
		void checkSyncPipe_();
		// Handle polling Servers
		void checkServers_();
		// Handle polling Clients
		void checkClients_();
		// Return 0 on success
		// Return -1 on handled error: client is deleted
		// Return 1 on handled error: client should still receive a response
		int handleClientPollin_(ClientHandler *, pollfd *);
		// Return 0 on success
		// Return 1 on success: client is deleted
		// Return -1 on handled error: client is deleted
		int handleClientPollout_(ClientHandler *, pollfd *);
		// Return retrieve a socket by its socket_fd (identifier)
		pollfd *getSocket_(int socket_fd_);
		// Log request status
		void handleRequest_(ClientHandler *);
		// Log request status
		void handleRequest_(ClientHandler *, const std::exception *);
		// Log response status
		void logResponse_(ClientHandler *);
		std::vector<ServerManager> servers_;
		std::map<int, ServerManager *> servers_map_;
		std::vector<ClientHandler *> clients_;
		std::vector<pollfd> sockets_;
		int syncPipe_[2];
		pollfd syncPoll_;
		bool isSyncing_;
	public:
		Runtime(const std::vector<ServerConfig>&, size_t maxClients);
		~Runtime();

		// Member functions

		// Start runtime
		void runServers();
		// Stop runtime
		void closeServers();
		// Get sockets as `vector<pollfd>` reference
		std::vector<pollfd>& getSockets();
		// Get clients as `vector<ClientHandler *>` reference
		std::vector<ClientHandler *>& getClients();
		// Force a Sync on poll
		// @note Will write on SyncPipe[1] which is checked by Poll
		void Sync();
};

#endif