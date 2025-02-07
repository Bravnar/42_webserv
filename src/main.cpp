
#include <iostream>
#include "classes/ConfigManager.hpp"
#include "classes/ServerManager.hpp"

int	main( int ac, char **av ) {
	ConfigManager	manager;
	std::vector<ServerManager *> servers;

	if (ac == 2) {
		manager = ConfigManager(av[1]);
	} else {
		try {
			manager = ConfigManager("./config_files/nginx_test.conf");
		} catch(const std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	manager.printConfig();
	{
		const std::vector<ServerConfig>& serversConf = manager.getServers();
		for(std::vector<ServerConfig>::const_iterator it = serversConf.begin(); it != serversConf.end(); it++) {
			servers.push_back(new ServerManager(*it));
		}
	}
	{
		for(std::vector<ServerManager *>::iterator it = servers.begin(); it != servers.end(); it++) {
			Logger::info("Running ") << (*it)->getConfig().getServerNames()[0] << std::endl;
			(*it)->runServer();
		}
	}
	return 0;
}
