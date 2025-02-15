
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

int ConfigManager::getMinTimeout( void ) const {
    int minimum = std::numeric_limits<int>::max() ;
    for ( std::vector<ServerConfig>::const_iterator it = _servers.begin(); it != _servers.end() ; ++it ) {
        minimum = std::min(minimum, it->getTimeout()) ;
    }
    return minimum ;
}

void ConfigManager::printConfig( void ) const {
    const int   totalWidth = 80 ;

    for (std::vector<ServerConfig>::const_iterator it = _servers.begin() ; it != _servers.end() ; ++it ) {
        // SERVER INFO BLOCK
        int serverNum = std::distance(_servers.begin(), it) ;
        std::cout << C_RED "┌" C_BOLD "[SERVER " << serverNum << "]" C_RESET C_RED "────────────────────────────────────────────────────────────────────┐" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "┌" C_BOLD " SERVER CONFIG " C_RESET C_ORANGE "───────────────────────────────────────────────────────────┐" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(21) << C_BOLD << "Host:\t\t\t" C_RESET << std::left << std::setw(totalWidth - 43) << it->getHost() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(21) << C_BOLD << "Port:\t\t\t" C_RESET << std::left << std::setw(totalWidth - 43) << it->getPort() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(21) << C_BOLD << "Name:\t\t\t" C_RESET << std::left << std::setw(totalWidth - 43) << it->getServerNames()[0] << C_ORANGE "│" C_RED " │" C_RESET << std::endl ; // takes only the first name!
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(21) << C_BOLD << "Root:\t\t\t" C_RESET << std::left << std::setw(totalWidth - 43) << it->getServerRoot() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(21) << C_BOLD << "Body Limit:\t\t" C_RESET << std::left << std::setw(totalWidth - 43) << it->getClientBodyLimit() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(21) << C_BOLD << "Max Clients:\t\t" C_RESET << std::left << std::setw(totalWidth - 43) << it->getMaxClients() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(21) << C_BOLD << "Timeout:\t\t" C_RESET << std::left << std::setw(totalWidth - 43) << it->getTimeout() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(21) << C_BOLD << "Default file:\t\t" C_RESET << std::left << std::setw(totalWidth - 43) << it->getIndex() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "└──────────────────────────────────────────────────────────────────────────┘" C_RED " │" C_RESET << std::endl ;

        // ERRORS BLOCK
        std::cout << C_RED "│ " C_MAGENTA "┌" C_BOLD " ERROR PAGES " C_RESET C_MAGENTA "─────────────────────────────────────────────────────────────┐" C_RED " │" C_RESET << std::endl ;
        for (std::map<int, std::string>::const_iterator it1 = it->getErrorPages().begin() ; it1 != it->getErrorPages().end() ; ++it1) {
            std::cout << C_RED "│ " C_MAGENTA "│" C_RESET << std::left << std::setw(21) << C_BOLD << it1->first << ":\t\t\t" C_RESET << std::left << std::setw(totalWidth - 43) << it1->second << C_MAGENTA "│" C_RED " │" C_RESET << std::endl ;
        }
        std::cout << C_RED "│ " C_MAGENTA "└──────────────────────────────────────────────────────────────────────────┘" C_RED " │" C_RESET << std::endl ;

        // LOCATIONS BLOCK
        int first = 1 ;
        for (std::vector<RouteConfig>::const_iterator it2 = it->getRoutes().begin() ; it2 != it->getRoutes().end() ; ++it2) {
            if (first)
                std::cout << C_RED "│ " C_CYAN "┌" C_BOLD " LOCATIONS " C_RESET C_CYAN "───────────────────────────────────────────────────────────────┐" C_RED " │" C_RESET << std::endl ;
            else
                std::cout << C_RED "│ " C_CYAN "├──────────────────────────────────────────────────────────────────────────┤" C_RED " │" C_RESET << std::endl ;
            // LocationPath
            std::cout << C_RED "│ " C_CYAN "│" C_RESET 
                << std::left << std::setw(21) << C_BOLD << "Path:\t\t\t" C_RESET 
                << std::left << std::setw(totalWidth - 43) << it2->getPath() 
                << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            // LocationRoot
            std::cout << C_RED "│ " C_CYAN "│" C_RESET 
                << std::left << std::setw(21) << C_BOLD << "Root:\t\t\t" C_RESET 
                << std::left << std::setw(totalWidth - 43) 
                << it2->getLocationRoot() << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            #if LOGGER_DEBUG > 0
                std::cout << C_RED "│ " C_CYAN "│" C_RESET << std::left << std::setw(21) << C_BOLD << "FinalRoot:\t\t" C_RESET << std::left << std::setw(totalWidth - 43) << it2->getFinalPath() << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            #endif
            // Allowed Methods
            std::string methods_str ;
            for (std::vector<std::string>::const_iterator it3 = it2->getMethods().begin(); it3 != it2->getMethods().end(); ++it3) {
                if (it3 != it2->getMethods().begin())
                methods_str += ", " ;
                methods_str += *it3 ;
            }
            std::cout << C_RED "│ " C_CYAN "│" C_RESET 
                << std::left << std::setw(21) << C_BOLD << "Methods:\t\t" C_RESET 
                << std::left << std::setw(totalWidth - 43) << methods_str 
                << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            // Directory Listing ON/OFF
            std::cout << C_RED "│ " C_CYAN "│" C_RESET 
                << std::left << std::setw(21) << C_BOLD << "Directory Listing:\t" C_RESET 
                << std::left << std::setw(totalWidth - 43) 
                << (it2->isDirectoryListingEnabled() ? "ON" : "OFF") 
                << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            // Is CGI ON/OFF
            if (it2->getIsCgi()) 
                std::cout << C_RED "│ " C_CYAN "│" C_RESET 
                    << std::left << std::setw(21) << C_BOLD << "CGI:\t\t\t" C_RESET 
                    << std::left << std::setw(totalWidth - 43) 
                    << (it2->getIsCgi() ? "ON" : "OFF" ) << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            // Is Upload Accepted ON/OFF
            if (it2->isUploadAccepted())
                std::cout << C_RED "│ " C_CYAN "│" C_RESET 
                    << std::left << std::setw(21) << C_BOLD << "Upload accepted:\t" C_RESET 
                    << std::left << std::setw(totalWidth - 43) 
                    << (it2->isUploadAccepted() ? "ON" : "OFF" ) << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            // Upload Path
            if (it2->getUploadPath() != "/uploads" || it2->isUploadAccepted())
                std::cout << C_RED "│ " C_CYAN "│" C_RESET 
                    << std::left << std::setw(21) << C_BOLD << "Upload Path:\t\t" C_RESET 
                    << std::left << std::setw(totalWidth - 43) 
                    << it2->getLocationRoot() + it2->getUploadPath() 
                    << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            // Return Path (redirect)
            if (!it2->getReturn().empty())
                std::cout << C_RED "│ " C_CYAN "│" C_RESET 
                    << std::left << std::setw(21) << C_BOLD << "Redirect:\t\t" C_RESET 
                    << std::left << std::setw(totalWidth - 43) 
                    << it2->getReturn()
                    << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            first = 0 ;
        }
        // Footer
        std::cout << C_RED "│ " C_CYAN "└──────────────────────────────────────────────────────────────────────────┘" C_RED " │" C_RESET << std::endl ;

        std::cout << C_RED "└──────────────────────────────────────────────────────────────────────────────┘" C_RESET << std::endl << std::endl << std::endl ;
    }
}
