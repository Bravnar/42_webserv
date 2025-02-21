#include "CgiHandler.hpp"
#include "ClientHandler.hpp"
#include <cstring>

/* Constructors / Destructors */

CgiHandler::CgiHandler( ClientHandler *client ) : 
_cgiStrVect(),
_client( client ),
_method( client->getRequest().getMethod() ) { }

CgiHandler::CgiHandler( const CgiHandler& other ) :
_cgiStrVect( other._cgiStrVect ),
_client( other._client ),
_method( other._method ) { }

CgiHandler& CgiHandler::operator=( const CgiHandler& other ) { 
	if (this != &other ) {
		_cgiStrVect = other._cgiStrVect ;
		_client =  other._client  ;
		_method =  other._method  ;
	}
	return *this ; 
}

CgiHandler::~CgiHandler( void ) { }

/* Helper private functions */

void	CgiHandler::_setEnvVariables( void ) {

	std::cout << _client->getRequest().getMethod() << std::endl ;

	_cgiStrVect.push_back("GATEWAY_INTERFACE=CGI/1.1") ;
	_cgiStrVect.push_back("REQUEST_METHOD=" + _client->getRequest().getMethod()) ;
	_cgiStrVect.push_back("SCRIPT_NAME=" + _client->getRequest().getUrl()) ;

	for	( size_t i = 0 ; i < _cgiStrVect.size() ; i++ ) {
		_envp.push_back(const_cast<char *>(_cgiStrVect[i].c_str())) ;
	}
	_envp.push_back(NULL) ;
}

/* Main function run */

std::string	CgiHandler::run( void ) {

	std::cout << "Entering run()" << std::endl ;
	if (_method != "GET" && _method != "POST") throw std::runtime_error("Invalid method for CGI.") ;

	_setEnvVariables() ;
	// _buildCgiEnv() ;
	// if (_method == "GET") _executeCgiGet() ;
	// if (_method == "POST") _executeCgiPost() ;
	
	return "Work in Progress\n" ;
}


/* CgiHandler::CgiHandler( void ) : 
_client() { } // in private should not be called


CgiHandler::CgiHandler( const HttpResponse& response) :
_client(response) { 
	#if LOGGER_DEBUG > 0
		Logger::debug("CgiHandler created successfully") << std::endl ;
	#endif
	std::cout << "Created successfully\n" ; 
}

CgiHandler::CgiHandler( const CgiHandler& other ) :
_client(other._client) { 
	for ( size_t i = 0; i < other._envp.size(); ++i ) {
		_envp.push_back(new char[std::strlen(other._envp[i]) + 1]) ;
		std::strcpy(_envp.back(), other._envp[i]) ;
	}
}

CgiHandler&	CgiHandler::operator=( const CgiHandler& other ) {
	if ( this != &other ) {
		for ( size_t i = 0; i < _envp.size(); ++i ) {
			delete[] _envp[i] ;
		}
		_envp.clear() ;
		_client = other._client ;

		for ( size_t i = 0; i < other._envp.size(); ++i ) {
			_envp.push_back(new char[std::strlen(other._envp[i]) + 1]) ;
			std::strcpy(_envp.back(), other._envp[i]) ;
		}
	}
	return *this ;
} 

CgiHandler::~CgiHandler( void ) {
	for ( size_t i = 0; i < _envp.size(); ++i ) {
		delete[] _envp[i] ;
	}
	_envp.clear() ;
	#if LOGGER_DEBUG > 0
		Logger::debug("Cgi cleared and exited, may cause memory issues for now") << std::endl ;
	#endif
} */

// void	CgiHandler::_setEnvVariables( void ) {
// 	std::vector<std::string>	tmp ;

// 	tmp.push_back("REQUEST_METHOD=" + _client.getRe)
// }
