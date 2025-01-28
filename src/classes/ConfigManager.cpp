
# include "ConfigManager.hpp"

ConfigManager::ConfigManager( void ) {
	// should not be called
}

ConfigManager::ConfigManager( const std::string &filePath ) : _confPath(filePath) {
	_parseConfig() ;
}

ConfigManager::ConfigManager( const ConfigManager &other ) :
_serverConf(other._serverConf),
_confPath(other._confPath) { } 

ConfigManager& ConfigManager::operator=( const ConfigManager &other ) {
	if (this != &other) {
		_serverConf = other._serverConf ;
		_confPath = other._confPath ;
	}
	return *this ;
}

ConfigManager::~ConfigManager( void ) {
	// kill
}

void ConfigManager::_parseConfig( void ) {
	try { _serverConf = ConfigParser::parse( _confPath ) ; }
	catch(const std::exception& e) { std::cerr << e.what() << '\n'; }
}