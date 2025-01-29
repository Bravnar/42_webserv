# include "ServerConfig.hpp"

/* Default Constructor */
ServerConfig::ServerConfig( void ) :
_host(""),
_port(8080),
_serverNames(),
_errorPages(),
_clientBodyLimit(1024),
_routes() { }

/* Copy Constructor */
ServerConfig::ServerConfig( const ServerConfig &other ) :
_host(other._host),
_port(other._port),
_serverNames(other._serverNames),
_errorPages(other._errorPages),
_routes(other._routes) { }

/* Assignment Operator Overload */
ServerConfig& ServerConfig::operator=( const ServerConfig &other ) {
	if (this != &other) {
		_host = other._host ;
		_port = other._port ;
		_serverNames = other._serverNames ;
		_errorPages = other._errorPages ;
	} return *this ;
}

/* Destructor */
ServerConfig::~ServerConfig( void ) { }

/* SETTERS */
void	ServerConfig::setHost( const std::string &host ) { _host = host ; }
void	ServerConfig::setPort( int port ) { _port = port ; }
void	ServerConfig::addServerName( const std::string& name ) { _serverNames.push_back(name) ; }
void	ServerConfig::addErrorPage( int errorCode, const std::string& errorFile ) { 
	_errorPages[errorCode] = errorFile ; 
}
void	ServerConfig::setClientBodyLimit( int limit ) { _clientBodyLimit = limit ; }
void	ServerConfig::addRoute( const RouteConfig& route ) { _routes.push_back(route) ; }

/* GETTERS */
