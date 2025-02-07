#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include "RouteConfig.hpp"

class ServerConfig
{
	private:
		std::string					_host;
		int							_port;
		std::vector<std::string>	_serverNames ;
		std::map<int, std::string>	_errorPages ;
		int							_clientBodyLimit ;
		std::string					_defaultFile ;
		std::vector<RouteConfig>	_routes ;

	public:
		ServerConfig();
		ServerConfig( const ServerConfig &other ) ;
		ServerConfig& operator=( const ServerConfig &other ) ;
		~ServerConfig();

		void	setHost( const std::string& host ) ;
		void	setPort( int port ) ;
		void	addServerName( const std::string& name ) ;
		void	addErrorPage( int errorCode, const std::string& errorFile ) ;
		void	setClientBodyLimit( int limit ) ;
		void	setDefaultFile( const std::string &file ) ;
		void	addRoute( const RouteConfig& route ) ;


		const	std::string&				getHost() const ;
		int									getPort() const ;
		const	std::vector<std::string>&	getServerNames() const ;
		const 	std::map<int, std::string>&	getErrorPages() const ;
		int									getClientBodyLimit() const ;
		const std::string					&getDefaultFile( ) const ;
		std::vector<RouteConfig>			&getRoutes();
		const std::vector<RouteConfig>		getRoutes() const ;
};

#endif