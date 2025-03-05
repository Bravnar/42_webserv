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

		std::ostream& fatal(const std::string&) const ;
		std::ostream& error(const std::string&) const ;
		std::ostream& warning(const std::string&) const ;
		std::ostream& info(const std::string&) const ;
		std::ostream& debug(const std::string&) const ;

		// Initializes server
		void initializeServers_(const std::vector<ServerConfig>&);
		// Get binded server for virtual host
		ServerManager *getHost(const ServerManager&);
		// Properly handle exit
		void handleExit_();
		// Handle polling Servers
		void checkServersSocket_();
		// Handle polling Clients
		void checkClientsSockets_();
		// Return 0 on success
		// Return -1 on handled error: client is deleted
		// Return 1 on handled error: client should still receive a response
		int handleClientPollin_(ClientHandler *, pollfd *);
		// Return 0 on success
		// Return 1 on success: client is deleted
		// Return -1 on handled error: client is deleted
		int handleClientPollout_(ClientHandler *, pollfd *);
		// Return 0 on success
		// Return 1 on success: client is deleted
		int handleClientPollhup_(ClientHandler *, pollfd *);
		// Return retrieve a socket by its socket_fd (identifier)
		pollfd *getSocket_(int socket_fd_);
		// Log request status
		void handleRequest_(ClientHandler *);
		// Log request status
		void handleRequest_(ClientHandler *, const std::string&);
		// Log response status
		void logResponse_(ClientHandler *);
		const ConfigManager& config_;
		std::vector<ServerManager> servers_;
		std::map<int, ServerManager *> servers_map_;
		std::map<int, ClientHandler *> clients_;
		std::vector<pollfd> sockets_;
		unsigned long long lat_tick_;

		static Runtime	*ptr ;
		static void	signalHandler_( int ) ;
		void	setUpSignalHandler_() ;


		public:
		Runtime(const ConfigManager&);
		~Runtime();

		// Member functions

		// Start runtime
		void runServers();
		// Stop runtime
		void closeServers();
		// Get sockets as `vector<pollfd>` reference
		std::vector<pollfd>& getSockets();
		// Get clients as `vector<ClientHandler *>` reference
		std::map<int, ClientHandler *>& getClients();
};

#endif