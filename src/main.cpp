
#include <iostream>
#include "classes/ConfigManager.hpp"

int	main( void ) {

	std::cout << "Hello World!" << std::endl ;

	ConfigManager	manager("./config_files/ini_test.conf") ;

	return 0;
}
