#include "./classes/ServerManager.hpp"
#include <iostream>
#include <poll.h>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>

int	main( void ) {
	ServerManager server(8080);
	if (!server.isHealthy())
		return 1;
	server.run();
	return 0;
}