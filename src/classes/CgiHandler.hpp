#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <iostream>
# include <string>
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
		std::vector<std::string>			_outputHeaders ;
		std::string							_outputBody ;

		

		void						_setEnvVariables() ;
		void						_execProcess( const std::string &scriptPath ) ;
		void						_parseOutput( const std::string &output ) ;
		// std::string				_executeCgiGet() ;
		// std::string				_executeCgiPost() ;
		
		CgiHandler( void ) ;

	public:
		CgiHandler( ClientHandler *client, const RouteConfig *route ) ;
		CgiHandler( const CgiHandler &other ) ;
		CgiHandler& operator=( const CgiHandler &other ) ;
		~CgiHandler( void ) ;

		std::string			run() ;
		const std::vector<std::string>	&getOutputHeaders( void ) const ;
		const std::string				&getOutputBody( void ) const ;

} ;

#endif