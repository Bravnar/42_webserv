#include "CgiHandler.hpp"
#include <cstring>

CgiHandler::CgiHandler( void ) : 
_scriptPath(""),
// _request(),
_response() { } // in private should not be called

// CgiHandler::CgiHandler( const HttpRequest& request, const std::string &scriptPath ) :
// _scriptPath(scriptPath),
// _request(request) { 
// 	#if LOGGER_DEBUG > 0
// 		Logger::debug("CgiHandler created successfully") << std::endl ;
// 	#endif
// 	std::cout << "Created successfully\n" ; 
// }

CgiHandler::CgiHandler( const HttpResponse& response, const std::string &scriptPath ) :
_scriptPath(scriptPath),
// _request(request),
_response(response) { 
	#if LOGGER_DEBUG > 0
		Logger::debug("CgiHandler created successfully") << std::endl ;
	#endif
	std::cout << "Created successfully\n" ; 
}

CgiHandler::CgiHandler( const CgiHandler& other ) :
_scriptPath(other._scriptPath),
// _request(other._request),
_response(other._response) { 
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

		_scriptPath = other._scriptPath ;
		// _request = other._request ;
		_response = other._response ;

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
}

std::string	CgiHandler::run( void ) {
	std::cout << "Hello, from the C-G-Aaaaaaaiee" << std::endl ; // like Adele's hello from the other siiiide

	
	return "Work in Progress\n" ;
}