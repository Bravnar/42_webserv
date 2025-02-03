#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include <string>
# include <vector>
# include <iostream>
# include <fstream>
# include <sstream>
# include <istream>
# include "ServerConfig.hpp"


class ConfigParser {
	private:
		typedef	void (*ServerConfigHandler)(ServerConfig&, const std::string&) ;
		typedef void (*RouteConfigHandler)(RouteConfig&, const std::string&) ;
			
		static const std::map<ServerConfigHandler, std::string>	_serverKeyHandlers ;
		static const std::map<RouteConfigHandler, std::string> _routeKeyHandlers ;
		
		ConfigParser();

	public:
		ConfigParser( const ConfigParser &other ) ;
		ConfigParser& operator=( const ConfigParser &other ) ;
		~ConfigParser();
		
		static std::vector<ServerConfig> parse( const std::string &filePath ) ;
		static void	parseKeyValue(ServerConfig& server, RouteConfig* route, const std::string& line) ;

} ;

#endif