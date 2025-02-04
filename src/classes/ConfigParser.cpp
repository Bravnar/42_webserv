# include "ConfigParser.hpp"

/* Construtctors / Deconstructors ------------------------------------- */
ConfigParser::ConfigParser( void ) {
	_initMaps() ; 
} 
ConfigParser::ConfigParser( const ConfigParser &other ) { ( void ) other ; }
ConfigParser& ConfigParser::operator=( const ConfigParser &other ) {
	if ( this != &other ) ( void ) other ; 
	return *this ;
}
ConfigParser::~ConfigParser( void ) { }
/* -------------------------------------------------------------------- */

/* Setters for function pointers -------------------------------------- */

void	setHost( ServerConfig& server, const std::string &str ) {
	server.setHost( str ) ;
}

void	setPort( ServerConfig& server, const std::string &str ) {
	char	*endptr = NULL ;
	int		result ;

	result = std::strtol( str.c_str(), &endptr, 10 ) ;
	if (result < 0 && result > 65535 || endptr) 
		throw std::runtime_error("Error: Invalid Port.") ;
	server.setPort(result) ;
}

void	setServerNames( ServerConfig& server, const std::string &str) {
	return ;
}

void	setClientBodyLimit( ServerConfig& server, const std::string &str) {
	return ;
}

void	setDefaultFile( ServerConfig& server, const std::string &str) {
	return ;
}

void	setRoot( RouteConfig& server, const std::string &str) {
	return ;
}

void	setMethods( RouteConfig& server, const std::string &str) {
	return ;
}

void	setDirectoryListing( RouteConfig& server, const std::string &str) {
	return ;
}

void	setCgiPath( RouteConfig& server, const std::string &str) {
	return ;
}

void	setAcceptUploads( RouteConfig& server, const std::string &str) {
	return ;
}

void	setUploadPath( RouteConfig& server, const std::string &str) {
	return ;
}


/* MAP INITIALISATION ------------------------------------------------- */

std::map<std::string, ServerConfigHandler> ConfigParser::_serverKeyHandlers ;
std::map<std::string, RouteConfigHandler> ConfigParser::_routeKeyHandlers ;


void	ConfigParser::_initMaps( void ) {
	_serverKeyHandlers.clear() ;
	_routeKeyHandlers.clear() ;

	_serverKeyHandlers["host"] = setHost ;
	_serverKeyHandlers["port"] = setPort ;
	_serverKeyHandlers["server_name"] = setServerNames ;
	_serverKeyHandlers["client_body_limit"] = setClientBodyLimit ;
	_serverKeyHandlers["default_file"] = setDefaultFile ;

	_routeKeyHandlers["root"] = setRoot ;
	_routeKeyHandlers["methods"] = setMethods ;
	_routeKeyHandlers["directory_listing"] = setDirectoryListing ;
	_routeKeyHandlers["cgi_path"] = setCgiPath ;
	_routeKeyHandlers["accept_uploads"] = setAcceptUploads ;
	_routeKeyHandlers["upload_path"] = setUploadPath ;
}

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

void	ConfigParser::parseKeyValue( ServerConfig& server, RouteConfig* route, const std::string& line, bool isError ) {
	std::istringstream	iss( line ) ;
	std::string			key, value ;
	iss >> key ;

	std::getline(iss, value, ';') ;
	value = trim(value) ;
	(void) route ;
	(void) server ;
	
	if (route || isError) {
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
	bool						inErrorBlock = false ;
	// int							serverCount = 0 ;
	// int							locationCount = 0 ;
	// int							errorCount = 0 ;
	
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
			// serverCount++ ;
			continue ;
		}

		if (line == "}" || line.find("}") != std::string::npos) {
			if (inLocationBlock) inLocationBlock = false ;
			else if (inServerBlock) inServerBlock = false ;
			else if (inErrorBlock) inErrorBlock = false ;
			continue ;
		}

		if (line == "errors {") {
			if (inErrorBlock) throw std::runtime_error("Error: Indented error blocks are not allowed.") ;
			if (currentServer) {
				std::cout << "IDENTIFIED ERROR LOCATION !" << std::endl ;
				inErrorBlock = true ; 
				// errorCount++ ;
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
				// locationCount++ ;
			}
			continue ;
		}

		if (inServerBlock && !inErrorBlock && !inLocationBlock ) parseKeyValue(*currentServer, NULL, line, false) ;
		else if (inServerBlock && inErrorBlock && !inLocationBlock) {
			parseKeyValue(*currentServer, NULL, line, true) ;
		} else if (inLocationBlock) {
			RouteConfig&	route = currentServer->getRoutes()[currentServer->getRoutes().size() - 1] ;
			parseKeyValue(*currentServer, &route, line, false) ;
		}
	}
	// std::cout << "Server blocks counts: " << serverCount << std::endl ;
	// std::cout << "Location blocks counts: " << locationCount << std::endl ;
	// std::cout << "Error blocks counts: " << errorCount << std::endl ;
	return servers ;
}



// const std::map<ConfigParser::ServerConfigHandler, std::string> ConfigParser::_serverKeyHandlers = {
// 	{"host", setHost},
// 	{"port", setPort},
// 	{"server_name", setServerName},
// 	{"error_page", setErrorPage},
// 	{"client_limit", setClientLimit}, // add to ServerConfig
// 	{"client_body_limit", setClientBodyLimit},
// } ;

// const std::map<ConfigParser::RouteConfigHandler, std::string> ConfigParser::_routeKeyHandlers = {
// 	{"root", setHost},
// 	{"methods", setPort},
// 	{"directory_listing", setServerName},
// 	{"default_file", setErrorPage},
// 	{"cgi_path", setClientBodyLimit},
// 	{"accept_uploads", setAcceptUploads},
// 	{"upload_path", setUploadPath}
// } ;