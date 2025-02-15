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
		bool						_isDefault ;
		std::string					_serverRoot ;
		unsigned long long			_clientBodyLimit ;
		int							_maxClients ;
		int							_timeout ;
		std::string					_index ;
		std::map<int, std::string>	_errorPages ;
		std::vector<RouteConfig>	_routes ;

	public:
		ServerConfig();
		ServerConfig( const ServerConfig &other ) ;
		ServerConfig& operator=( const ServerConfig &other ) ;
		~ServerConfig();

		void	setHost( const std::string& host ) ;
		void	setPort( int port ) ;
		void	addServerName( const std::string& name ) ;
		void	setIsDefault( bool io ) ;
		void	setServerRoot( const std::string& root ) ;
		void	setClientBodyLimit( unsigned long long limit ) ;
		void 	setMaxClients( int maxClients ) ;
		void	setTimeout( int timeout ) ;
		void	setIndex( const std::string &file ) ;
		void	addErrorPage( int errorCode, const std::string& errorFile ) ;
		void	addRoute( const RouteConfig& route ) ;


		const	std::string					&getHost( ) const ;
		int									getPort( ) const ;
		const	std::vector<std::string>	&getServerNames( ) const ;
		bool								getIsDefault( ) const ;
		const	std::string					&getServerRoot( ) const ;
		unsigned long long					getClientBodyLimit( ) const ;
		int									getMaxClients( ) const ;
		int									getTimeout( ) const ;
		const std::string					&getIndex( ) const ;
		const 	std::map<int, std::string>&	getErrorPages( ) const ;
		std::vector<RouteConfig>			&getRoutes( ) ; 
		const std::vector<RouteConfig>		&getRoutes( ) const ;

		void								clearServerNames() ;
};

#endif