#include "CgiHandler.hpp"
#include <cstring>

CgiHandler::CgiHandler( void ) : 
_scriptPath(""),
_request() { } // in private should not be called

CgiHandler::CgiHandler( const HttpRequest& request, const std::string &scriptPath ) :
_scriptPath(scriptPath),
_request(request) { std::cout << "Created successfully\n" ; }

CgiHandler::CgiHandler( const CgiHandler& other ) :
_scriptPath(other._scriptPath),
_request(other._request) { 
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
		_request = other._request ;

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
}