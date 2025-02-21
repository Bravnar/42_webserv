#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "ClientHandler.hpp"
# include <iostream>
# include <vector>
# include <map>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>

class CgiHandler {
	private:

		std::vector<char *>		_envp ; // for execve
		const ClientHandler		&_client ;
		

		void				_setEnvVariables() ;
		std::vector<char *>	_buildCgiEnv() ;
		std::string			_executeCgiGet() ;
		std::string			_executeCgiPost() ;
		
		CgiHandler( void ) ;

	public:
		CgiHandler( const ClientHandler& client ) ;
		// CgiHandler( const HttpResponse& response) ;
		CgiHandler( const CgiHandler &other ) ;
		CgiHandler& operator=( const CgiHandler &other ) ;
		~CgiHandler( void ) ;

		std::string			run() ;

} ;

#endif