#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include <string>
# include <vector>
# include <map>
# include <algorithm>
# include <iostream>
# include <fstream>
# include <sstream>
# include <istream>
# include <unistd.h>
# include "ServerConfig.hpp"

typedef	void (*ServerConfigHandler)(ServerConfig&, const std::string&) ;
typedef void (*RouteConfigHandler)(RouteConfig&, const std::string&) ;

class ConfigParser {
	private:
			
		static std::map<std::string, ServerConfigHandler>	_serverKeyHandlers ;
		static std::map<std::string, RouteConfigHandler> _routeKeyHandlers ;

		static void	_initMaps( void ) ;
		
		ConfigParser();

	public:
		ConfigParser( const ConfigParser &other ) ;
		ConfigParser& operator=( const ConfigParser &other ) ;
		~ConfigParser();
		

		static std::vector<ServerConfig> parse( const std::string &filePath ) ;
		static void	parseKeyValue(ServerConfig& server, RouteConfig* route, const std::string& line, bool isError) ;

} ;

#endif