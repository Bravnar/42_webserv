# include "ServerConfig.hpp"

/* Default Constructor */
ServerConfig::ServerConfig( void ) :
_host("0.0.0.0"),
_port(8080),
_serverNames(),
_serverRoot(""),
_clientBodyLimit(1024),
_maxClients(100),
_index("/index.html"),
_errorPages(),
_routes() { 
	_serverNames.push_back("default"); 
} // adds "default", clears if names are given

/* Copy Constructor */
ServerConfig::ServerConfig( const ServerConfig &other ) :
_host(other._host),
_port(other._port),
_serverNames(other._serverNames),
_serverRoot(other._serverRoot),
_clientBodyLimit(other._clientBodyLimit),
_maxClients(other._maxClients),
_index(other._index),
_errorPages(other._errorPages),
_routes(other._routes) { }

/* Assignment Operator Overload */
ServerConfig& ServerConfig::operator=( const ServerConfig &other ) {
	if (this != &other) {
		_host = other._host ;
		_port = other._port ;
		_serverNames = other._serverNames ;
		_serverRoot = other._serverRoot ;
		_clientBodyLimit = other._clientBodyLimit ;
		_maxClients = other._maxClients ;
		_index = other._index ;
		_errorPages = other._errorPages ;
		_routes = other._routes ;
	} return *this ;
}

/* Destructor */
ServerConfig::~ServerConfig( void ) { }

/* SETTERS */
void	ServerConfig::setHost( const std::string &host ) { _host = host ; }
void	ServerConfig::setPort( int port ) { _port = port ; }
void	ServerConfig::addServerName( const std::string& name ) { _serverNames.push_back(name) ; }
void	ServerConfig::setServerRoot( const std::string& root ) { _serverRoot = root ; }
void	ServerConfig::setClientBodyLimit( unsigned long long limit ) { _clientBodyLimit = limit ; }
void	ServerConfig::setMaxClients( int maxClients ) { _maxClients = maxClients ; }
void	ServerConfig::setIndex( const std::string &file ) { _index = file ; }
void	ServerConfig::addErrorPage( int errorCode, const std::string& errorFile ) { 
	_errorPages[errorCode] = errorFile ; 
}
void	ServerConfig::addRoute( const RouteConfig& route ) { _routes.push_back(route) ; }

/* GETTERS */

const std::string&					ServerConfig::getHost( ) const { return _host ; }
int									ServerConfig::getPort( ) const { return _port ; }
const std::vector<std::string>&		ServerConfig::getServerNames( ) const { return _serverNames ; }
const std::string&					ServerConfig::getServerRoot( ) const { return _serverRoot ; }
const std::map<int, std::string>&	ServerConfig::getErrorPages( ) const { return _errorPages ;}
unsigned long long					ServerConfig::getClientBodyLimit( ) const { return _clientBodyLimit ; }
const std::string&					ServerConfig::getIndex( ) const { return _index ; } 
std::vector<RouteConfig>&			ServerConfig::getRoutes( ) { return _routes ; }
const std::vector<RouteConfig>&		ServerConfig::getRoutes( ) const { return _routes ; }

void								ServerConfig::clearServerNames( ) { _serverNames.clear() ; }