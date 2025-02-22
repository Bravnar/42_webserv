#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <iostream>
# include <vector>
# include <map>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <exception>
# include "../utils/Logger.hpp"
# include "../utils/colors.h"

class ClientHandler ;
class RouteConfig ;

class CgiHandler {
	private:

		std::vector<std::string>			_cgiStrVect ;
		std::vector<char *>					_envp ;
		ClientHandler						*_client ;
		const RouteConfig					*_route ;
		std::string							_method ;
		std::string							_cgi ;
		

		void						_setEnvVariables() ;
		void						_execProcess( const std::string &scriptPath ) ;
		// std::string				_executeCgiGet() ;
		// std::string				_executeCgiPost() ;
		
		CgiHandler( void ) ;

	public:
		CgiHandler( ClientHandler *client, const RouteConfig *route ) ;
		CgiHandler( const CgiHandler &other ) ;
		CgiHandler& operator=( const CgiHandler &other ) ;
		~CgiHandler( void ) ;

		std::string			run() ;

} ;

#endif