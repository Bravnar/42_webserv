
#include <iostream>
#include <poll.h>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>

int	main( void ) {

	std::cout << "Hello World!" << std::endl ;
	int server_fd;
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cerr << "cannot create socket" << std::endl;
	}
	else
		std::cout << "socket " << server_fd << " created." << std::endl;
	struct sockaddr_in addrv4;
	struct sockaddr *address = (struct sockaddr* )&addrv4;
	int PORT = 8080;
	memset((char *)&addrv4, 0, sizeof(addrv4));
	addrv4.sin_family = AF_INET;
	addrv4.sin_addr.s_addr = htonl(INADDR_ANY);
	addrv4.sin_port = htons(PORT);
	if (bind(server_fd, address, sizeof(addrv4)) < 0) {
		std::cerr << "bind failed" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		close(server_fd);
		return 1;
	}
	else
		std::cout << "bind created" << std::endl;
	if (listen(server_fd, 3) < 0)
		std::cerr << "in listen" << std::endl;
	int new_socket;
	socklen_t addrlen = 0;
	if ((new_socket = accept(server_fd, address, &addrlen)))
		std::cerr << "in accept" << std::endl;
	char buffer[1024] = {0};
	int valread = read(new_socket, buffer, 1024);
	std::cout << buffer << std::endl;
	if (valread < 0)
		std::cout << "No bytes are there to read" << std::endl;
	std::string output = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world2!";
	write(new_socket, output.c_str(), output.length());
	close(new_socket);
	close(server_fd);
	return 0;
}