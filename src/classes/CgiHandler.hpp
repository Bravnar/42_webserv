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
# include "../utils/ParseUtils.hpp"
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
		std::map<std::string, std::string>	_outputHeaders ;
		std::string							_outputBody ;

		

		void						_setGetEnvVariables() ;
		void						_setPostEnvVariables() ;

		void						_execGet( const std::string &scriptPath ) ;
		void						_execPost( const std::string &scriptPath ) ;

		void						_parseOutput( const std::string &output ) ;
		
		CgiHandler( void ) ;

	public:
		CgiHandler( ClientHandler *client, const RouteConfig *route ) ;
		CgiHandler( const CgiHandler &other ) ;
		CgiHandler& operator=( const CgiHandler &other ) ;
		~CgiHandler( void ) ;

		void										run() ;
		const std::map<std::string, std::string>	&getOutputHeaders( void ) const ;
		long long int								getContentSize( void ) const ;
		const std::string							&getOutputBody( void ) const ;

} ;

#endif