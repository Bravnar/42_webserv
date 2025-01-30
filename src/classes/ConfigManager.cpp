
# include "ConfigManager.hpp"
# include "../utils/colors.h"

ConfigManager::ConfigManager( void ) {
	// should not be called
}

ConfigManager::ConfigManager( const std::string &filePath ) :
_servers(),
_confPath( filePath ) { 
	std::cout << "Constructor: created config manager with: " << filePath << std::endl ;
	_parseConfig() ;
}

ConfigManager::ConfigManager( const ConfigManager &other ) :
_servers(other._servers),
_confPath(other._confPath) { } 

ConfigManager& ConfigManager::operator=( const ConfigManager &other ) {
	if (this != &other) {
		_servers = other._servers ;
		_confPath = other._confPath ;
	}
	return *this ;
}

ConfigManager::~ConfigManager( void ) { }

void ConfigManager::_parseConfig( void ) {
	try { _servers = ConfigParser::parse( _confPath ) ; }
	catch(const std::exception& e) { std::cerr << "Error parsing config: " << e.what() << '\n'; }
}

void	ConfigManager::printConfig( ) const {
	for ( size_t i = 0; i < _servers.size(); ++i ) {
		std::cout << C_B_YELLOW "Server [" << i << "]:" C_RESET << std::endl ;
		std::cout << C_B_WHITE "HostName: " << _servers[i].getHost() << std::endl ;
		std::cout << "Port: " << _servers[i].getPort() << std::endl ;
		std::cout << "Server Names: " C_RESET << std::endl ;
		for ( size_t j = 0; j < _servers[i].getServerNames()[j].size(); ++j ) {
			std::cout << "\t" << _servers[i].getServerNames()[j] << std::endl ;
		}
		std::cout << C_B_WHITE "Error Pages: " C_RESET << std::endl ;
		for (std::map<int, std::string>::const_iterator it = _servers[i].getErrorPages().begin();
				it != _servers[i].getErrorPages().end();
				++it) {
			std::cout << "\t" << it->first << " : " << it->second << std::endl ;
 		}
		std::cout << C_B_WHITE "Client Body Limit: " C_RESET << _servers[i].getClientBodyLimit() << std::endl ;
		for (std::vector<RouteConfig>::const_iterator it = _servers[i].getRoutes().begin();
				it != _servers[i].getRoutes().end();
				++it ) {
			std::cout << "\t" << it->getPath() << std::endl ;
		}
	}
}