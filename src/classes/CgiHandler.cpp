#include "CgiHandler.hpp"
#include "ClientHandler.hpp"
#include "RouteConfig.hpp"
#include <cstring>

/* Constructors / Destructors */

CgiHandler::CgiHandler( ClientHandler *client, const RouteConfig *route ) : 
_cgiStrVect(),
_client( client ),
_route( route ),
_method( client->getRequest().getMethod() ),
_cgi( route->getCgi().first ),
_extension( route->getCgi().second ),
_outputHeaders(),
_outputBody("") { }

CgiHandler::CgiHandler( const CgiHandler& other ) :
_cgiStrVect( other._cgiStrVect ),
_client( other._client ),
_route( other._route ),
_method( other._method ),
_cgi( other._cgi ),
_extension( other._extension ),
_outputHeaders( other._outputHeaders ),
_outputBody( other._outputBody) { }

CgiHandler& CgiHandler::operator=( const CgiHandler& other ) { 
	if (this != &other ) {
		_cgiStrVect = other._cgiStrVect ;
		_client =  other._client  ;
		_route = other._route ;
		_method =  other._method  ;
		_cgi = other._cgi ;
		_extension = other._extension ;
		_outputHeaders = other._outputHeaders ;
		_outputBody = other._outputBody ;
	}
	return *this ; 
}

CgiHandler::~CgiHandler( void ) { _cgiStrVect.clear() ; _envp.clear() ;}

/* Helper private functions */

void	CgiHandler::_setPostEnvVariables( void ) {
	return ;
}

void	CgiHandler::_setGetEnvVariables( void ) {

	_cgiStrVect.clear() ;
	_envp.clear() ;

	_cgiStrVect.push_back("GATEWAY_INTERFACE=CGI/1.1") ;
	_cgiStrVect.push_back("REQUEST_METHOD=" + _client->getRequest().getMethod()) ;
	_cgiStrVect.push_back("SCRIPT_FILENAME=" + _route->getLocationRoot() + _client->getRequest().getUrl()) ;
	_cgiStrVect.push_back("SERVER_PROTOCOL=" + _client->getRequest().getHttpVersion()) ;
	_cgiStrVect.push_back("SERVER_SOFTWARE=PlaceHolder") ;
	_cgiStrVect.push_back("REDIRECT_STATUS=200") ;
	for	( size_t i = 0 ; i < _cgiStrVect.size() ; i++ ) {
		_envp.push_back(const_cast<char *>(_cgiStrVect[i].c_str())) ;
	}
	_envp.push_back(NULL) ;
}

void	CgiHandler::_parseOutput( const std::string &output ) {

	size_t	header_end = output.find("\r\n\r\n") ;
	size_t	offset = 4 ;
	if ( header_end == std::string::npos ) {
		header_end = output.find("\n\n") ;
		offset = 2 ;
	}

	std::string	headers = output.substr( 0, header_end ) ;
	_outputBody = output.substr( header_end + offset );

	_outputHeaders.clear() ;
	std::istringstream	headerStream(headers) ;
	std::string line ;
	while (std::getline(headerStream, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r') {
			line.resize(line.size() - 1) ;
		}
		line = replWhitespace(trim(line)) ;
		std::istringstream	iss(line) ;
		std::string	key, value ;
		iss >> key ;
		key = trim(key) ;
		if (key.at(key.size() - 1) == ':') key.resize(key.size() - 1) ;
		if (key == "Content-type") key = "Content-Type" ;
		std::cout << "Header key: " << key << std::endl ;
		std::getline( iss, value ) ;
		value = trim(value) ;
		_outputHeaders[key] = value ;
	}

	/* for (std::map<std::string, std::string>::iterator it = _outputHeaders.begin() ; it != _outputHeaders.end() ; it++) {
		std::cout << "KEY: " << it->first << " | " << "VALUE: " << it->second << std::endl ; 
	} */
}

void	CgiHandler::_execGet( const std::string &scriptPath ) {

	Logger::debug("Script path: ") << scriptPath << std::endl ;
	int 	pipefd[2] ;
	if (pipe(pipefd) == -1) throw std::runtime_error("Failed to create pipe.") ;

	pid_t	pid = fork() ;
	if ( pid == -1 ) throw std::runtime_error("Failed to fork process.") ;
	else if (pid == 0) {
		close( pipefd[0] ) ;
		dup2( pipefd[1], STDOUT_FILENO ) ;
		close( pipefd[1] ) ;  

		char	*av[] = { const_cast<char *>(_cgi.c_str()), const_cast<char *>(scriptPath.c_str()), NULL } ;
		execve(_cgi.c_str(), av, &_envp[0]) ;
		Logger::fatal("Execve failed.");
		exit(EXIT_FAILURE) ;
	} else {
		close(pipefd[1]) ;

		char		buffer[1024] ;
		std::string	output ;
		ssize_t		bytesRead ;

		while ((bytesRead = read( pipefd[0], buffer, sizeof(buffer) - 1)) > 0) output.append(buffer, bytesRead) ; 

		close( pipefd[0] ) ;
		/* clock_t	start = clock() ;
		while ( (static_cast<double>(clock() - start) / CLOCKS_PER_SEC * 1000) < 5000 ) */
			waitpid( pid, NULL, WNOHANG ) ;
		// TODO: handle infinite loop in the script
		_parseOutput( output ) ;
		// Logger::info("Content-Length: ") << this->getContentSize() << std::endl ;
		// Logger::info("CGI output:\n") << this->getOutputBody() << std::endl ;
	}
}

void	CgiHandler::_execPost( const std::string &scriptPath ) {
	return ; // temporary
	Logger::debug("Script path: ") << scriptPath << std::endl ;

	int	pipefd[2] ;
	if (pipe(pipefd) == -1) throw std::runtime_error( "Failed to create pipe." ) ;

	pid_t	pid = fork() ;
	if ( pid == -1 ) throw std::runtime_error( "Failed to fork process" ) ;
	else if ( pid == 0 ) {
		// child
	} else {
		//parent
	}
}

/* Main function run */

void	CgiHandler::run( void ) {

	if (_method != "GET" && _method != "POST") throw std::runtime_error("Invalid method for CGI.") ;
	if (_method == "GET") {
		_setGetEnvVariables() ;
		_execGet( _route->getLocationRoot() + _client->getRequest().getUrl() ) ;
	}
	else if (_method == "POST") {
		_setPostEnvVariables() ;
		_execPost( _route->getLocationRoot() + _client->getRequest().getUrl() ) ;
	}
	/* for ( size_t i = 0; i < _envp.size(); i++)
		std::cout << _envp[i] << std::endl ; */
}

/* Getters */

const std::map<std::string, std::string>&	CgiHandler::getOutputHeaders( void ) const { return _outputHeaders ; }
const std::string&							CgiHandler::getOutputBody( void ) const { return _outputBody ; }
long long int								CgiHandler::getContentSize( void ) const {

	char	*endptr = NULL ;
	long long int	result ;
	std::map<std::string, std::string>::const_iterator it = _outputHeaders.find("Content-Length") ;
	if (it == _outputHeaders.end()) throw std::runtime_error("Content-Length not found.") ;
	result = std::strtoll( it->second.c_str(), &endptr, 10 ) ;
	if (*endptr != '\0') throw std::runtime_error("Invalid Content-Length value.");
	return result ;
}

