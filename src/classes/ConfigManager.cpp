
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
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(20) << C_BOLD << " Body Limit:\t" C_RESET << std::left << std::setw(totalWidth - 35) << it->getClientBodyLimit() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "│" C_RESET << std::left << std::setw(20) << C_BOLD << " Default file:\t" C_RESET << std::left << std::setw(totalWidth - 35) << it->getDefaultFile() << C_ORANGE "│" C_RED " │" C_RESET << std::endl ;
        std::cout << C_RED "│ " C_ORANGE "└──────────────────────────────────────────────────────────────────────────┘" C_RED " │" C_RESET << std::endl ;

        //errors
        std::cout << C_RED "│ " C_MAGENTA "┌" C_BOLD " ERROR PAGES " C_RESET C_MAGENTA "─────────────────────────────────────────────────────────────┐" C_RED " │" C_RESET << std::endl ;
        for (std::map<int, std::string>::const_iterator it1 = it->getErrorPages().begin() ; it1 != it->getErrorPages().end() ; ++it1) {
            std::cout << C_RED "│ " C_MAGENTA "│" C_RESET << std::left << std::setw(20) << C_BOLD << it1->first << ":\t\t" C_RESET << std::left << std::setw(totalWidth - 35) << it1->second << C_MAGENTA "│" C_RED " │" C_RESET << std::endl ;
        }
        std::cout << C_RED "│ " C_MAGENTA "└──────────────────────────────────────────────────────────────────────────┘" C_RED " │" C_RESET << std::endl ;

        //locations
        std::cout << C_RED "│ " C_CYAN "┌" C_BOLD " LOCATIONS " C_RESET C_CYAN "───────────────────────────────────────────────────────────────┐" C_RED " │" C_RESET << std::endl ;
        for (std::vector<RouteConfig>::const_iterator it2 = it->getRoutes().begin() ; it2 != it->getRoutes().end() ; ++it2) {
            std::cout << C_RED "│ " C_CYAN "│" C_RESET << std::left << std::setw(20) << C_BOLD << " Path:\t\t" C_RESET << std::left << std::setw(totalWidth - 35) << it2->getPath() << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            std::cout << C_RED "│ " C_CYAN "│" C_RESET << std::left << std::setw(20) << C_BOLD << " Root:\t\t" C_RESET << std::left << std::setw(totalWidth - 35) << it2->getRoot() << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
            if (!it2->getMethods().empty())
                std::cout << C_RED "│ " C_CYAN "│" C_RESET << std::left << std::setw(20) << C_BOLD << " Methods:\t\t" C_RESET << std::left << std::setw(totalWidth - 35) << C_CYAN "│" C_RED " │" C_RESET << std::endl ;
        }



        std::cout << C_RED "└──────────────────────────────────────────────────────────────────────────────┘" C_RESET << std::endl << std::endl << std::endl ;
    }
}

/* void	ConfigManager::printConfig( void ) const {
	std::cout << C_YELLOW "\n============== SERVER CONFIGURATION ==============\n" C_RESET ;

	for ( size_t i = 0; i < _servers.size(); ++i ) {
		const ServerConfig& server = _servers[i] ;

		std::cout << C_BLUE "\n--------------------------------------------------\n" C_RESET ;
		std::cout << C_RED "SERVER [" << i << "]" C_RESET << std::endl ;
		std::cout << C_BLUE "--------------------------------------------------\n" C_RESET ;

		std::cout << C_BLUE "[HOST]\t\t" C_RESET << server.getHost() << std::endl ;
        std::cout << C_BLUE "[PORT]\t\t" C_RESET << server.getPort() << std::endl ;

		std::cout << C_BLUE "[SERVER_NAMES]\t" C_RESET;
        const std::vector<std::string>& serverNames = server.getServerNames();
        if (!serverNames.empty()) {
            for (size_t j = 0; j < serverNames.size(); ++j) {
                std::cout << serverNames[j] << (j < serverNames.size() - 1 ? ", " : "");
            }
        } else {
            std::cout << "(None)";
        }
        std::cout << std::endl ;

		std::cout << C_BLUE "[DEFAULT_FILE]\t" C_RESET << server.getDefaultFile() << std::endl ;
		std::cout << C_BLUE "[BODY_LIMIT]\t" C_RESET << server.getClientBodyLimit() << " bytes" << std::endl ;

		std::cout << C_BLUE "[ERROR_PAGES]\n" C_RESET;
        const std::map<int, std::string>& errorPages = server.getErrorPages();
        if (!errorPages.empty()) {
            for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
                std::cout << "\t" C_ORANGE << it->first << C_RESET " →\t" << it->second << std::endl ;
            }
        } else {
            std::cout << "\t(None configured)" << std::endl ;
        }
		const std::vector<RouteConfig>& routes = server.getRoutes();
        if (!routes.empty()) {
            std::cout << C_MAGENTA "\n[ROUTES]" C_RESET << std::endl ;
            for (size_t j = 0; j < routes.size(); ++j) {
                const RouteConfig& route = routes[j];

                std::cout << C_CYAN "\nROUTE [" << j << "] - Path: < " << route.getPath() << " >\n" C_RESET;
                std::cout << "-----------------------------------\n";
                std::cout << C_BLUE "[ROOT]\t\t" C_RESET << route.getRoot() << "\n";

                // Print Allowed Methods
                std::cout << C_BLUE "[METHODS]\t" C_RESET;
                const std::vector<std::string>& methods = route.getMethods();
                if (!methods.empty()) {
                    for (size_t k = 0; k < methods.size(); ++k) {
                        std::cout << methods[k] << (k < methods.size() - 1 ? ", " : "");
                    }
                } else {
                    std::cout << "(None)";
                }
                std::cout << std::endl;

                std::cout << C_BLUE "[DIR_LIST]\t" C_RESET << (route.isDirectoryListingEnabled() ? "ON" : "OFF") << "\n";

                if (!route.getCgiPath().empty()) {
                    std::cout << C_BLUE "[CGI_PATH]\t" C_RESET << route.getCgiPath() << std::endl ;
                }

                if (route.isUploadAccepted()) {
                    std::cout << C_BLUE "[UPL_ACCEPT]\t" C_RESET << "ON" << std::endl ;
                    std::cout << C_BLUE "[UPL_PATH]\t" C_RESET << route.getUploadPath() << std::endl ;
                }

                if (!route.getRedirect().empty()) {
                    std::cout << C_BLUE "[REDIRECT]\t" C_RESET << route.getRedirect() << std::endl ;
                }
            }
        } else {
            std::cout << "\nRoutes: (None configured)" << std::endl ;
        }

        std::cout << std::endl ;
    }

    std::cout << "==========================================\n";
} */