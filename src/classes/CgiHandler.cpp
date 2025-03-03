#include "CgiHandler.hpp"
#include "ClientHandler.hpp"
#include "RouteConfig.hpp"
#include <cstring>

# define ERR_CGI "encountered issue with the CGI script." 
# define ERR_CGI_TIMEOUT "CGI script timed out."

/* Constructors / Destructors */

CgiHandler::CgiHandler( ClientHandler *client, const RouteConfig *route ) : 
_cgiStrVect(),
_client( client ),
_route( route ),
_method( client->getRequest().getMethod() ),
_cgi( route->getCgi().first ),
_extension( route->getCgi().second ),
_outputHeaders(),
_outputBody(""),
_script( route->getLocationRoot() + client->getRequest().getUrl() ) { }

CgiHandler::CgiHandler( const CgiHandler& other ) :
_cgiStrVect( other._cgiStrVect ),
_client( other._client ),
_route( other._route ),
_method( other._method ),
_cgi( other._cgi ),
_extension( other._extension ),
_outputHeaders( other._outputHeaders ),
_outputBody( other._outputBody),
_script( other._script ) { }

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
		_script = other._script ;
	}
	return *this ; 
}

CgiHandler::~CgiHandler( void ) { _cgiStrVect.clear() ; _envp.clear() ;}

/* Helper private functions */

void	CgiHandler::_setPostEnvVariables( void ) {
	_cgiStrVect.clear() ;
	_envp.clear() ;

	std::string	phpIniPath = "" ;
	std::string contentType ;
	
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) { phpIniPath = std::string(cwd) + "/www/cgi-php/php.ini"; }
	
	if (_client->getRequest().getHeaders().find("Content-Type") != _client->getRequest().getHeaders().end())
		contentType = _client->getRequest().getHeaders().at("Content-Type") ;
	else
		contentType = "plain/text" ;

	_cgiStrVect.push_back("GATEWAY_INTERFACE=CGI/1.1") ;
	_cgiStrVect.push_back("REQUEST_METHOD=" + _client->getRequest().getMethod()) ;
	_cgiStrVect.push_back("SCRIPT_FILENAME=" + _script) ;
	_cgiStrVect.push_back("SERVER_PROTOCOL=" + _client->getRequest().getHttpVersion()) ;
	_cgiStrVect.push_back("SERVER_SOFTWARE=PlaceHolder") ;
	_cgiStrVect.push_back("REDIRECT_STATUS=200") ;
	_cgiStrVect.push_back("CONTENT_LENGTH=" + _client->getRequest().getHeaders().at("Content-Length")) ;
	_cgiStrVect.push_back("UPLOAD_DIR=" + _route->getFinalPath() + _route->getUploadPath()) ;
	_cgiStrVect.push_back("UPLOAD_DIR_PHP=." + _route->getUploadPath()) ;
	_cgiStrVect.push_back("CONTENT_TYPE=" + contentType) ;
	_cgiStrVect.push_back("HTTP_BOUNDARY=" + _client->getRequest().getBoundary()) ;
	_cgiStrVect.push_back("PHPRC=" + phpIniPath); 
	
	for	( size_t i = 0 ; i < _cgiStrVect.size() ; i++ ) {
		_envp.push_back(const_cast<char *>(_cgiStrVect[i].c_str())) ;
	}
	_envp.push_back(NULL) ;
}

void	CgiHandler::_setGetEnvVariables( void ) {

	_cgiStrVect.clear() ;
	_envp.clear() ;

	_cgiStrVect.push_back("GATEWAY_INTERFACE=CGI/1.1") ;
	_cgiStrVect.push_back("REQUEST_METHOD=" + _client->getRequest().getMethod()) ;
	_cgiStrVect.push_back("SCRIPT_FILENAME=" + _script) ;
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
		std::getline( iss, value ) ;
		value = trim(value) ;
		_outputHeaders[key] = value ;
	}
}

void	CgiHandler::_execGet( const std::string &scriptPath ) {

	Logger::debug("Script path: ") << scriptPath << std::endl ;
	int 	pipefd[2] ;
	if (pipe(pipefd) == -1) throw std::runtime_error("Failed to create pipe.") ;

	pid_t	pid = fork() ;
	if ( pid == -1 ) throw std::runtime_error("Failed to fork process.") ;
	else if (pid == 0) {
		close(STDERR_FILENO) ; // Ensure no error output from CGI scripts
		close( pipefd[0] ) ;
		dup2( pipefd[1], STDOUT_FILENO ) ;
		close( pipefd[1] ) ;  

		alarm(1) ;

		char	*av[] = { const_cast<char *>(_cgi.c_str()), const_cast<char *>(scriptPath.c_str()), NULL } ;
		execve(_cgi.c_str(), av, &_envp[0]) ;
		Logger::fatal("Execve failed.");
		exit(EXIT_FAILURE) ;
	} else {
		close(pipefd[1]) ;


		char		buffer[DF_MAX_BUFFER] ;
		std::string	output ;
		ssize_t		bytesRead ;

		while ((bytesRead = read( pipefd[0], buffer, sizeof(buffer) - 1)) > 0) output.append(buffer, bytesRead) ; 

		close( pipefd[0] ) ;

		int status ;
		time_t	startTime = time(NULL) ;
		while (waitpid( pid, &status, WNOHANG ) == 0 ) {
			if (time(NULL) - startTime > 1 ) {
				kill( pid, SIGKILL ) ;
				waitpid( pid, &status, 0 ) ;
				status = 408 ;
				break ;
			}
			usleep(100000) ;
		}
		if (status != 0)
			throw std::runtime_error(ERR_CGI) ;

		_parseOutput( output ) ;
	}
}

void	CgiHandler::_execPost( const std::string &scriptPath ) {

	Logger::debug("Script path: ") << scriptPath << std::endl ;

	int	inputPipe[2], outputPipe[2];
	if (pipe(inputPipe) == -1 || pipe(outputPipe) == -1 ) throw std::runtime_error( "Failed to create pipes." ) ;

	pid_t	pid = fork() ;
	if ( pid == -1 ) throw std::runtime_error( "Failed to fork process" ) ;
	else if ( pid == 0 ) {
		#ifndef LOGGER_DEBUG
			close(STDERR_FILENO) ; // Ensure no error output from CGI scripts
		#endif
		close(inputPipe[1]) ;
		dup2(inputPipe[0], STDIN_FILENO) ;
		close(inputPipe[0]) ;

		close(outputPipe[0]) ;
		dup2(outputPipe[1], STDOUT_FILENO) ;
		close(outputPipe[1]) ;

		alarm(1) ;

		char	*av[] = { const_cast<char *>(_cgi.c_str()), const_cast<char *>(scriptPath.c_str()), NULL } ;
		execve(_cgi.c_str(), av, &_envp[0]) ;
		Logger::fatal("Execve failed.") ;
		exit(EXIT_FAILURE) ;

	} else {
		close(inputPipe[0]) ;
		close(outputPipe[1]) ;

		std::string body = *_client->getRequest().getAllBody() ;
		write(inputPipe[1], body.c_str(), body.size()) ;
		close(inputPipe[1]) ;

		char		buffer[DF_MAX_BUFFER] ;
		std::string	output ;
		ssize_t		bytesRead ;
		int status ;

		while ((bytesRead = read( outputPipe[0], buffer, sizeof(buffer) - 1)) > 0) output.append(buffer, bytesRead) ;
		close(outputPipe[0]) ;

		time_t	startTime = time(NULL) ;
		while (waitpid( pid, &status, WNOHANG ) == 0 ) {
			if (time(NULL) - startTime > 1 ) {
				kill( pid, SIGKILL ) ;
				waitpid( pid, &status, 0 ) ;
				break ;
			}
			usleep(100000) ;
		}
		if (status != 0)
			throw std::runtime_error(ERR_CGI) ;
			
		_parseOutput( output ) ;
	}
}

/* Main function run */

void	CgiHandler::run( void ) {

	if (_method != "GET" && _method != "POST") throw std::runtime_error("Invalid method for CGI.") ;
	if (_method == "GET") {
		_setGetEnvVariables() ;
		_execGet( _script ) ;
	}
	else if (_method == "POST") {
		_setPostEnvVariables() ;
		_execPost( _script ) ;
	}
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

bool	CgiHandler::_checkShebang( const std::string& filePath ) {
	std::ifstream	file( filePath.c_str() ) ;
	if (!file.is_open()) { Logger::error("Failed to open file for shebang check.\n"); return false ; }
	if (file.peek() == std::ifstream::traits_type::eof()) return false ;
	std::string		line ;

	while (std::getline( file, line ) && line.empty())
		continue ;
	size_t	shebangPos = line.find("#!") ;
	std::string shebang = trim(line.substr(shebangPos + 2)) ;
	if (shebangPos == std::string::npos) return false ;
	if (shebangPos + 2 >= line.size()) return false ;
	return (shebang == _cgi) ;
}

bool	CgiHandler::isValidCgi( ) { 
	if (_script == _route->getFinalPath()) _script += _route->getIndex() ;
	size_t	dotPos = _script.find_last_of('.') ;
	std::string extension = ( dotPos != std::string::npos ) ? _script.substr(dotPos + 1) : "" ;
	return (extension == _extension || _checkShebang( _script )) ;
}
