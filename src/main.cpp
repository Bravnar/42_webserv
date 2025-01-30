
#include <iostream>
#include "classes/ConfigManager.hpp"

int	main( int ac, char **av ) {

	if (ac == 2) {
		ConfigManager	manager(av[1]) ;
		// ServerManager	server(manager) ;
		
	} else {
		ConfigManager	manager("./config_files/ini_test.conf") ;
	}

	return 0;
}
