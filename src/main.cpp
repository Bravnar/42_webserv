
#include <iostream>
#include "classes/ConfigManager.hpp"

int	main( int ac, char **av ) {
	if (ac == 2) {
		ConfigManager	manager(av[1]) ;
	} else {
		try {
			ConfigManager	manager("./config_files/nginx_test.conf") ;
			// manager.printConfig() ;
		} catch(const std::exception& e) {
			std::cerr << e.what() << std::endl ;
		}
	}

	return 0;
}
