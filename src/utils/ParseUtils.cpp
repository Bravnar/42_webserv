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
	std::istringstream	iss( str ) ;
	std::string			oneName ;
	while ( iss >> oneName ) server.addServerName( trim( oneName ) ) ;
}

void	setError( ServerConfig& server, const std::string &str ) {
	std::istringstream	iss( str ) ;
	std::string	errorCodeStr, errorPage ;

	while (iss >> errorCodeStr >> errorPage ) {
		char *endptr = NULL ;
		int	errorCode = std::strtol(errorCodeStr.c_str(), &endptr, 10) ;

		if (*endptr != '\0') throw std::runtime_error( "Invalid error code: " + errorCodeStr ) ;
		if ( errorCode < 100 || errorCode > 599 ) throw std::runtime_error( "Invalid HTTP error code.") ;

		server.addErrorPage( errorCode, errorPage ) ;
	}
}

void	setClientBodyLimit( ServerConfig& server, const std::string &str) {
	char		*endptr = NULL ;
	size_t		result ;

	result = std::strtol( str.c_str(), &endptr, 10 ) ;
	if (*endptr != '\0') throw std::runtime_error("invalid Client Body Limit: non-numeric value.") ;
	if (result <= 0 || result > 2147483647) 
		throw std::runtime_error("invalid client body limit.") ;
	server.setClientBodyLimit(result) ;
}

void	setDefaultFile( ServerConfig& server, const std::string &str) {
	server.setDefaultFile( str ) ;
}

void	setRoot( RouteConfig& route, const std::string &str) {
	route.setRoot( str ) ;
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

void	setDirectoryListing( RouteConfig& route, const std::string &str) {
	if ( str == "on" ) route.setDirectoryListing(true) ;
	else if ( str == "off" ) route.setDirectoryListing(false) ;
	else throw std::runtime_error("invalid directory listing value: must be 'on' or 'off'.") ;
}

void	setCgiPath( RouteConfig& route, const std::string &str) {
	route.setCgiPath( str ) ;
}

void	setRedirect( RouteConfig& route, const std::string &str) {
	route.setRedirect( str ) ;
}


void	setAcceptUploads( RouteConfig& route, const std::string &str) {
	if ( str == "on" ) route.setAcceptUploads(true) ;
	else if ( str == "off" ) route.setAcceptUploads(false) ;
	else throw std::runtime_error("invalid accept uploads value: must be 'on' or 'off'.") ;
}

void	setUploadPath( RouteConfig& route, const std::string &str) {
	route.setUploadPath( str ) ;
}