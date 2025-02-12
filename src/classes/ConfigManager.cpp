
# include "ConfigManager.hpp"
# include "../utils/colors.h"

ConfigManager::ConfigManager( void ) {
	// should not be called
}

ConfigManager::ConfigManager( const std::string &filePath ) :
_servers(),
_confPath( filePath ) { _parseConfig() ; }

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

const std::vector<ServerConfig>& ConfigManager::getServers() const {
    return _servers ;
}

void ConfigManager::printConfig( void ) const {
    const int   totalWidth = 80 ;

    for (std::vector<ServerConfig>::const_iterator it = _servers.begin() ; it != _servers.end() ; ++it ) {
        int serverNum = std::distance(_servers.begin(), it) ;
        std::cout << C_RED "┌" C_BOLD "[SERVER " << serverNum << "]" C_RESET C_RED "────────────────────────────────────────────────────────────────────┐" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "┌" C_BOLD " SERVER CONFIG " C_RESET C_ORANGE "───────────────────────────────────────────────────────────┐" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(20) << C_BOLD << " Name:\t\t" C_RESET << std::left << std::setw(totalWidth - 35) << it->getServerNames()[0] << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(20) << C_BOLD << " Host:\t\t" C_RESET << std::left << std::setw(totalWidth - 35) << it->getHost() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(20) << C_BOLD << " Port:\t\t" C_RESET << std::left << std::setw(totalWidth - 35) << it->getPort() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(20) << C_BOLD << " Root:\t\t" C_RESET << std::left << std::setw(totalWidth - 35) << it->getServerRoot() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(20) << C_BOLD << " Body Limit:\t" C_RESET << std::left << std::setw(totalWidth - 35) << it->getClientBodyLimit() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(20) << C_BOLD << " Default file:\t" C_RESET << std::left << std::setw(totalWidth - 35) << it->getIndex() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "└──────────────────────────────────────────────────────────────────────────┘" C_RED " │" C_RESET << std::endl ;

        //errors
        std::cout << C_RED "│ " C_MAGENTA "┌" C_BOLD " ERROR PAGES " C_RESET C_MAGENTA "─────────────────────────────────────────────────────────────┐" C_RED " │" C_RESET << std::endl ;
        for (std::map<int, std::string>::const_iterator it1 = it->getErrorPages().begin() ; it1 != it->getErrorPages().end() ; ++it1) {
            std::cout << C_RED "│ " C_MAGENTA "│" C_RESET << std::left << std::setw(21) << C_BOLD << it1->first << ":\t\t" C_RESET << std::left << std::setw(totalWidth - 35) << it1->second << C_MAGENTA "│" C_RED " │" C_RESET << std::endl ;
        }
        std::cout << C_RED "│ " C_MAGENTA "└──────────────────────────────────────────────────────────────────────────┘" C_RED " │" C_RESET << std::endl ;

        //locations
        int first = 1 ;
        
        // std::cout << C_RED "│ " C_CYAN "┌" C_BOLD " LOCATIONS " C_RESET C_CYAN "───────────────────────────────────────────────────────────────┐" C_RED " │" C_RESET << std::endl ;
        for (std::vector<RouteConfig>::const_iterator it2 = it->getRoutes().begin() ; it2 != it->getRoutes().end() ; ++it2) {
            if (first)
                std::cout << C_RED "│ " C_CYAN "┌" C_BOLD " LOCATIONS " C_RESET C_CYAN "───────────────────────────────────────────────────────────────┐" C_RED " │" C_RESET << std::endl ;
            else
                std::cout << C_RED "│ " C_CYAN "├──────────────────────────────────────────────────────────────────────────┤" C_RED " │" C_RESET << std::endl ;
            std::cout << C_RED "│ " C_CYAN "│" C_RESET 
                << std::left << std::setw(20) << C_BOLD << " Path:\t\t" C_RESET 
                << std::left << std::setw(totalWidth - 35) << it2->getPath() 
                << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            std::cout << C_RED "│ " C_CYAN "│" C_RESET 
                << std::left << std::setw(20) << C_BOLD << " Root:\t\t" C_RESET 
                << std::left << std::setw(totalWidth - 35) 
                << it2->getLocationRoot() << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            // std::cout << C_RED "│ " C_CYAN "│" C_RESET << std::left << std::setw(20) << C_BOLD << " FinalRoot:\t" C_RESET << std::left << std::setw(totalWidth - 35) << it2->getFinalPath() << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            if (it2->getUploadPath() != "/uploads")
                std::cout << C_RED "│ " C_CYAN "│" C_RESET 
                    << std::left << std::setw(20) << C_BOLD << " Upload Path:\t" C_RESET 
                    << std::left << std::setw(totalWidth - 35) 
                    << it2->getLocationRoot() + it2->getUploadPath() 
                    << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            std::string methods_str ;
            for (std::vector<std::string>::const_iterator it3 = it2->getMethods().begin(); it3 != it2->getMethods().end(); ++it3) {
                if (it3 != it2->getMethods().begin())
                    methods_str += ", " ;
                methods_str += *it3 ;
            }
            std::cout << C_RED "│ " C_CYAN "│" C_RESET 
                << std::left << std::setw(20) << C_BOLD << " Methods:\t" C_RESET 
                << std::left << std::setw(totalWidth - 35) << methods_str 
                << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            std::cout << C_RED "│ " C_CYAN "│" C_RESET 
                << std::left << std::setw(20) << C_BOLD << " Dir Listing:\t" C_RESET 
                << std::left << std::setw(totalWidth - 35) 
                << (it2->isDirectoryListingEnabled() ? "ON" : "OFF") 
                << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            first = 0 ;
        }
        std::cout << C_RED "│ " C_CYAN "└──────────────────────────────────────────────────────────────────────────┘" C_RED " │" C_RESET << std::endl ;



        std::cout << C_RED "└──────────────────────────────────────────────────────────────────────────────┘" C_RESET << std::endl << std::endl << std::endl ;
    }
}
