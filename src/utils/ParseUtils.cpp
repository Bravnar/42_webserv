# include "../classes/ConfigParser.hpp"

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

/* Setters for function pointers -------------------------------------- */

/* SERVER CONFIG ------------------------------------------------------ */

void	setHost( ServerConfig& server, const std::string &str ) {
	server.setHost( str ) ;
}

void	setPort( ServerConfig& server, const std::string &str ) {
	char	*endptr = NULL ;
	int		result ;

	result = std::strtol( str.c_str(), &endptr, 10 ) ;
	if (*endptr != '\0') throw std::runtime_error("invalid Port: non-numeric value.") ;
	if (result < 0 || result > 65535) 
		throw std::runtime_error("invalid Port: must be between 0 and 65535.") ;
	server.setPort(result) ;
}

void	setServerNames( ServerConfig& server, const std::string &str) {
	if (std::find(server.getServerNames().begin(), server.getServerNames().end(), "default") != server.getServerNames().end()) {
		server.clearServerNames() ;
		server.setIsDefault(false) ;
	}
	std::istringstream	iss( str ) ;
	std::string			oneName ;
	while ( iss >> oneName ) server.addServerName( trim( oneName ) ) ;
}

void	setServerRoot( ServerConfig& server, const std::string &str ) {
	server.setServerRoot( str ) ;
}

void	setClientBodyLimit( ServerConfig& server, const std::string &str) {
	char					*endptr = NULL ;
	unsigned long long		result ;

	result = std::strtoull( str.c_str(), &endptr, 10 ) ;
	if (*endptr != '\0') throw std::runtime_error("invalid Client Body Limit: non-numeric value.") ;
	if (result <= 0 || result > 2147483647) 
		throw std::runtime_error("invalid client body limit.") ;
	server.setClientBodyLimit(result) ;
}

void	setMaxClients( ServerConfig& server, const std::string &str ) {
	char		*endptr = NULL ;
	size_t		result ;

	result = std::strtol( str.c_str(), &endptr, 10 ) ;
	if (*endptr != '\0') throw std::runtime_error("invalid Max Clients: non-numeric value.") ;
	if (result <= 0 || result > 2147483647) 
		throw std::runtime_error("invalid client body limit.") ;
	server.setMaxClients(result) ;
} 

void	setTimeout( ServerConfig& server, const std::string &str ) {
	char		*endptr = NULL ;
	size_t		result ;

	result = std::strtol( str.c_str(), &endptr, 10 ) ;
	if (*endptr != '\0') throw std::runtime_error("invalid Timeout: non-numeric value.") ;
	if (result <= 0 || result > 2147483647) 
		throw std::runtime_error("invalid timeout limit.") ;
	server.setTimeout(result) ;
} 

void	setError( ServerConfig& server, const std::string &str ) {
	std::string trimmedStr = trim(str) ;

	size_t endPos = trimmedStr.find(';') ;
	if (endPos == std::string::npos) throw std::runtime_error("directive must end with ';': " + str) ;

	std::string needed = trimmedStr.substr(0, endPos) ;
	needed = trim(needed) ;
	
	std::istringstream	iss(needed) ;
	std::string errorCodeStr, errorPath ;

	iss >> errorCodeStr ;
	std::getline(iss, errorPath) ;
	errorPath = trim(errorPath) ;

	char *endptr = NULL ;
	int errorCode = std::strtol(errorCodeStr.c_str(), &endptr, 10) ;
	if (*endptr != '\0') throw std::runtime_error("Invalid error code: " + errorCodeStr) ;
	if (errorCode < 100 || errorCode > 599) throw std::runtime_error("Invalid HTTP error code.") ;
	if (errorPath.empty()) throw std::runtime_error("Error page path cannot be empty.") ;

	server.addErrorPage(errorCode, errorPath) ;
}


void	setIndex( ServerConfig& server, const std::string &str) {
	server.setIndex( str ) ;
}

/* ROUTE CONFIG ------------------------------------------------------- */

void	setLocationRoot( RouteConfig& route, const std::string &str) {
	route.setLocationRoot( str ) ;
}

void	setMethods( RouteConfig& route, const std::string &str) {
	std::istringstream	iss( str ) ;
	std::string			oneMethod ;
	while ( iss >> oneMethod ) {
		if (oneMethod != "GET" && oneMethod != "POST" && oneMethod != "DELETE")
			throw std::runtime_error( "invalid HTTP method: " + oneMethod ) ;
		route.addMethod( oneMethod ) ;
	}
}

void	setCgi( RouteConfig& route, const std::string &str ) {
	if (access( str.c_str(), F_OK | X_OK ) ) throw std::runtime_error("invalid cgi-file provided.");
	route.setCgi( str ) ;
}

void	setReturn( RouteConfig& route, const std::string &str ) {
	std::cout << "SALUT!" << std::endl ;
	route.setReturn( str ) ;
}

void	setDirectoryListing( RouteConfig& route, const std::string &str) {
	if ( str == "on" ) route.setDirectoryListing(true) ;
	else if ( str == "off" ) route.setDirectoryListing(false) ;
	else throw std::runtime_error("invalid directory listing value: must be 'on' or 'off'.") ;
}

void	setAcceptUploads( RouteConfig& route, const std::string &str) {
	if ( str == "on" ) route.setAcceptUploads(true) ;
	else if ( str == "off" ) route.setAcceptUploads(false) ;
	else throw std::runtime_error("invalid accept uploads value: must be 'on' or 'off'.") ;
}

void	setUploadPath( RouteConfig& route, const std::string &str) {
	route.setUploadPath( str ) ;
}