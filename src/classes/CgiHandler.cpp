#include "CgiHandler.hpp"
#include <cstring>

CgiHandler::CgiHandler( void ) : 
_response() { } // in private should not be called


CgiHandler::CgiHandler( const HttpResponse& response) :
_response(response) { 
	#if LOGGER_DEBUG > 0
		Logger::debug("CgiHandler created successfully") << std::endl ;
	#endif
	std::cout << "Created successfully\n" ; 
}

CgiHandler::CgiHandler( const CgiHandler& other ) :
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

// void	CgiHandler::_setEnvVariables( void ) {
// 	std::vector<std::string>	tmp ;

// 	tmp.push_back("REQUEST_METHOD=" + _response.getRe)
// }

std::string	CgiHandler::run( void ) {

	std::cout << C_RED "\n\n####################### CGI #########################" C_RESET << std::endl ;
	
	std::cout << "getResLine: " << _response.getResLine() << std::endl ;
	std::cout << "status is int: " << _response.getStatus() << std::endl ;
	std::cout << "status message: " << _response.getStatusMsg() << std::endl ;
	std::cout << "response URL: " << *_response.getUrl() << std::endl ; 
	std::cout << "str()" << _response.str() << std::endl ;

	std::cout << C_CYAN "HEADERS:" C_RESET << std::endl ; 
	for (std::map<std::string, std::string>::iterator it = _response.getHeaders().begin(); it != _response.getHeaders().end(); it++ ) {
		std::cout << it->first << " : " << it->second << std::endl ;
	} 

	std::cout << C_RED "\n#####################################################" C_RESET << std::endl ;
	
	return "Work in Progress\n" ;
}