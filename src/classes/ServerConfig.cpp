# include "ServerConfig.hpp"

/* Default Constructor */
ServerConfig::ServerConfig( void ) :
_host("0.0.0.0"),
_port(80),
_serverNames(),
_errorPages(),
_clientBodyLimit(1024),
_defaultFile(""),
_routes() { }

/* Copy Constructor */
ServerConfig::ServerConfig( const ServerConfig &other ) :
_host(other._host),
_port(other._port),
_serverNames(other._serverNames),
_errorPages(other._errorPages),
_clientBodyLimit(other._clientBodyLimit),
_defaultFile(other._defaultFile),
_routes(other._routes) { }

/* Assignment Operator Overload */
ServerConfig& ServerConfig::operator=( const ServerConfig &other ) {
	if (this != &other) {
		_host = other._host ;
		_port = other._port ;
		_serverNames = other._serverNames ;
		_errorPages = other._errorPages ;
		_clientBodyLimit = other._clientBodyLimit ;
		_defaultFile = other._defaultFile ;
		_routes = other._routes ;
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
void	ServerConfig::setDefaultFile( const std::string &file ) { _defaultFile = file ; }
void	ServerConfig::addRoute( const RouteConfig& route ) { _routes.push_back(route) ; }

/* GETTERS */

const std::string&					ServerConfig::getHost( ) const { return _host ; }
int									ServerConfig::getPort( ) const { return _port ; }
const std::vector<std::string>&		ServerConfig::getServerNames( ) const { return _serverNames ; }
const std::map<int, std::string>&	ServerConfig::getErrorPages( ) const { return _errorPages ;}
int									ServerConfig::getClientBodyLimit( ) const { return _clientBodyLimit ; }
const std::string&					ServerConfig::getDefaultFile( ) const { return _defaultFile ; } 
std::vector<RouteConfig>&			ServerConfig::getRoutes( ) { return _routes ; }
const std::vector<RouteConfig>		ServerConfig::getRoutes( ) const { return _routes ; }