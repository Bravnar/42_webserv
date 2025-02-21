#include "CgiHandler.hpp"
#include <cstring>

CgiHandler::CgiHandler( const ClientHandler& client ) : _client( client ) { }
CgiHandler::CgiHandler( const CgiHandler& other ) : _client( other._client ) { }
CgiHandler& CgiHandler::operator=( const CgiHandler& other ) { return *this ; }

std::string	CgiHandler::run( void ) {

	std::cout << C_RED "\n\n####################### CGI #########################" C_RESET << std::endl ;
	
	std::cout << _client.getRequest().getMethod() << std::endl ;

	std::cout << C_RED "\n#####################################################" C_RESET << std::endl ;
	
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
