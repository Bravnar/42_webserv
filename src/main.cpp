
#include <iostream>
#include "classes/ConfigManager.hpp"
#include "classes/ServerManager.hpp"
#include "classes/Runtime.hpp"
#include "classes/CgiHandler.hpp"

int	main( int ac, char **av ) {
	ConfigManager	manager;
	std::vector<ServerManager *> servers;

	if (ac == 2) {
		manager = ConfigManager(av[1]);
	} else {
		try { manager = ConfigManager("./config_files/default.conf"); }
		catch(const std::exception& e) { std::cerr << e.what() << std::endl; }
	}

	manager.printConfig();

	Runtime runtime(manager);
	runtime.runServers();
	return 0;
}
