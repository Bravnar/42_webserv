#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <iostream>
# include <vector>
# include <map>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <exception>
# include "../utils/colors.h"

class ClientHandler ;

class CgiHandler {
	private:

		std::vector<std::string>	_cgiStrVect ;
		std::vector<char *>			_envp ; // for execve
		ClientHandler				*_client ;
		std::string					_method ;
		

		void						_setEnvVariables() ;
		// std::vector<char *>		_buildCgiEnv() ;
		// std::string				_executeCgiGet() ;
		// std::string				_executeCgiPost() ;
		
		CgiHandler( void ) ;

	public:
		CgiHandler( ClientHandler *client ) ;
		CgiHandler( const CgiHandler &other ) ;
		CgiHandler& operator=( const CgiHandler &other ) ;
		~CgiHandler( void ) ;

		std::string			run() ;

} ;

#endif