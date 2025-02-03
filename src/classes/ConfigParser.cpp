# include "ConfigParser.hpp"

/* Construtctors / Deconstructors ------------------------------------- */
ConfigParser::ConfigParser( void ) { } 
ConfigParser::ConfigParser( const ConfigParser &other ) { ( void ) other ; }
ConfigParser& ConfigParser::operator=( const ConfigParser &other ) {
	if ( this != &other ) ( void ) other ; 
	return *this ;
}
ConfigParser::~ConfigParser( void ) { }
/* -------------------------------------------------------------------- */

/* MAP INITIALISATION ------------------------------------------------- */

/* const std::map<ConfigParser::ServerConfigHandler, std::string> ConfigParser::_serverKeyHandlers = {
	{"host", setHost},
	{"port", setPort},
	{"server_name", setServerName},
	{"error_page", setErrorPage},
	{"client_body_limit", setClientBodyLimit}
} ;

const std::map<ConfigParser::RouteConfigHandler, std::string> ConfigParser::_routeKeyHandlers = {
	{"root", setHost},
	{"methods", setPort},
	{"directory_listing", setServerName},
	{"default_file", setErrorPage},
	{"cgi_path", setClientBodyLimit},
	{"accept_uploads", setAcceptUploads},
	{"upload_path", setUploadPath}
} ; */

/* SERVER CONFIGURATION HANDLERS -------------------------------------- */

/* ROUTE CONFIGURATION HANDLERS --------------------------------------- */



/* Helper Functions */
std::string	trim( const std::string& str ) {
	size_t first = str.find_first_not_of(" \t") ;
	if (first == std::string::npos) return "" ;
	size_t last = str.find_last_not_of(" \t") ;
	return str.substr(first, last - first + 1) ;
}

std::string	replWhitespace( const std::string& str ) {
	std::string result = "" ;
	bool	inSpace = false ;

	for (size_t i = 0; i < str.size(); ++i) {
		if (std::isspace(str[i])) {
			if (!inSpace) {
				result += ' ' ;
				inSpace = true ;
			}
		} else {
			result += str[i] ;
			inSpace = false ;
		}
	}
	return result ;
}

void	ConfigParser::parseKeyValue( ServerConfig& server, RouteConfig* route, const std::string& line ) {
	std::istringstream	iss( line ) ;
	std::string			key, value ;
	iss >> key ;

	std::getline(iss, value, ';') ;
	value = trim(value) ;
	(void) route ;
	(void) server ;
	
	if (route) {
		std::cout << "\t" << "KEY: " << key << " VALUE: " << value << std::endl ; 
	}
	else 
		std::cout << "KEY: " << key << " VALUE: " << value << std::endl ;



	/* SET UP ACCEPTED KEYS */
	/* HAVE A LOOK WITH MAP AND FUNCTION POINTERS TO MAKE EASY PARSING*/


	// std::getline(iss, value, ';') ;
	// value = trim(value) ;

	// if (key == "host") server.setHost( value ) ;
}

/* Main Parse method */
std::vector<ServerConfig> ConfigParser::parse( const std::string &filePath ) {

	if ( filePath.substr( filePath.find_last_of(".") + 1) != "conf" ) {
		throw std::runtime_error( "Error: Invalid config extension, expected .conf" ) ;
	} 
	
	std::ifstream	file(filePath.c_str()) ;
	if ( !file ) {
		throw std::runtime_error( "Error: Could not open config file" ) ;
	} 

	std::vector<ServerConfig>	servers;
	ServerConfig*				currentServer = NULL ;
	bool						inServerBlock = false ;
	bool						inLocationBlock = false ;
	int							serverCount = 0 ;
	int							locationCount = 0 ;
	
	std::string	line ;
	while (std::getline( file, line )) {
		line = replWhitespace(trim(line)) ;
		if (line.empty() || line.at(0) == '#') continue ;
		
		// std::cout << "Processing line: " << line << std::endl ;

		if (line == "server {") {
			if (inServerBlock) throw std::runtime_error("Error: Indented server blocks are not allowed.") ;
			std::cout << "IDENTIFIED SERVER BLOCK !" << std::endl ;
			servers.push_back(ServerConfig()) ;
			currentServer = &servers[servers.size() - 1] ;
			inServerBlock = true ;
			serverCount++ ;
			continue ;
		}

		if (line == "}" || line.find("}") != std::string::npos) {
			if (inLocationBlock) {
				inLocationBlock = false ;
			} else if (inServerBlock) {
				inServerBlock = false ;
			}
			continue ;
		}

		if (line.find("location ") == 0 && line[line.size() - 1] == '{') {
			if (inLocationBlock) throw std::runtime_error("Error: Indented location blocks are not allowed.") ;
			if (currentServer) {
				RouteConfig route ;
				route.setPath(trim(line.substr(9, line.size() - 10))) ;
				std::cout << "IDENTIFIED LOCATION " << route.getPath() << " !"<< std::endl ;
				currentServer->addRoute(route) ;
				inLocationBlock = true ;
				locationCount++ ;
			}
			continue ;
		}

		if (inServerBlock && !inLocationBlock) {
			parseKeyValue(*currentServer, NULL, line) ;
		}
		else if (inLocationBlock) {
			RouteConfig&	route = currentServer->getRoutes()[currentServer->getRoutes().size() - 1] ;
			parseKeyValue(*currentServer, &route, line) ;
		}
	}
	std::cout << "Server blocks counts: " << serverCount << std::endl ;
	std::cout << "Location blocks counts: " << locationCount << std::endl ;
	return servers ;
}
