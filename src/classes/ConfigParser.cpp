# include "ConfigParser.hpp"
# include "../utils/ParseUtils.hpp"

/* Construtctors / Deconstructors ------------------------------------- */
ConfigParser::ConfigParser( void ) { } 
ConfigParser::ConfigParser( const ConfigParser &other ) { ( void ) other ; }
ConfigParser& ConfigParser::operator=( const ConfigParser &other ) {
	if ( this != &other ) ( void ) other ; 
	return *this ;
}
ConfigParser::~ConfigParser( void ) { }

/* MAP INITIALISATION ------------------------------------------------- */

std::map<std::string, ServerConfigHandler> ConfigParser::_serverKeyHandlers ;
std::map<std::string, RouteConfigHandler> ConfigParser::_routeKeyHandlers ;


void	ConfigParser::_initMaps( void ) {
	_serverKeyHandlers.clear() ;
	_routeKeyHandlers.clear() ;

	_serverKeyHandlers["host"] = setHost ;
	_serverKeyHandlers["port"] = setPort ;
	_serverKeyHandlers["server_name"] = setServerNames ;
	_serverKeyHandlers["errors"] = setError ;
	_serverKeyHandlers["client_body_limit"] = setClientBodyLimit ;
	_serverKeyHandlers["default_file"] = setDefaultFile ;

	_routeKeyHandlers["root"] = setRoot ;
	_routeKeyHandlers["methods"] = setMethods ;
	_routeKeyHandlers["directory_listing"] = setDirectoryListing ;
	_routeKeyHandlers["cgi_path"] = setCgiPath ;
	_routeKeyHandlers["redirect"] = setRedirect ;
	_routeKeyHandlers["accept_uploads"] = setAcceptUploads ;
	_routeKeyHandlers["upload_path"] = setUploadPath ;
}


void	ConfigParser::parseKeyValue( ServerConfig& server, RouteConfig* route, const std::string& line, bool isError) {
	if (isError) {
		_serverKeyHandlers.find("errors")->second( server, line ) ;
		return ;
	}
	std::istringstream	iss( line ) ;
	std::string			key, value ;
	iss >> key ;
	key = trim(key) ;
	std::getline(iss, value, ';') ;
	value = trim(value) ;

	if (route) {
		if (_routeKeyHandlers.find(key) == _routeKeyHandlers.end())
			throw std::runtime_error("invalid route key detected: " + key ) ;
		_routeKeyHandlers.find(key)->second(*route, value) ;
		return ;
	}

	if (_serverKeyHandlers.find(key) == _serverKeyHandlers.end())
		throw std::runtime_error("invalid server key detected: " + key ) ;

	_serverKeyHandlers.find(key)->second(server, value) ;
}

/* Main Parse method */
std::vector<ServerConfig> ConfigParser::parse( const std::string &filePath ) {
	_initMaps() ;

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
	
	std::string	line ;
	while (std::getline( file, line )) {
		line = replWhitespace(trim(line)) ;
		if (line.empty() || line.at(0) == '#') continue ;

		if (line == "server {") {
			if (inServerBlock) throw std::runtime_error("Error: Indented server blocks are not allowed.") ;
			// std::cout << "IDENTIFIED SERVER BLOCK !" << std::endl ;
			servers.push_back(ServerConfig()) ;
			currentServer = &servers[servers.size() - 1] ;
			inServerBlock = true ;
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
				// std::cout << "IDENTIFIED ERROR LOCATION !" << std::endl ;
				inErrorBlock = true ; 
			}
			continue ;
		}

		if (line.find("location ") == 0 && line[line.size() - 1] == '{') {
			if (inLocationBlock) throw std::runtime_error("Error: Indented location blocks are not allowed.") ;
			if (currentServer) {
				RouteConfig route ;
				route.setPath(trim(line.substr(9, line.size() - 10))) ;
				// std::cout << "IDENTIFIED LOCATION " << route.getPath() << " !"<< std::endl ;
				currentServer->addRoute(route) ;
				inLocationBlock = true ;
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
	return servers ;
}
