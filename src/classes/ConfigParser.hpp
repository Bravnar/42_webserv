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
		ConfigParser();

		void	_parseKeyValue( ServerConfig& server, RouteConfig* route, const std::string& line ) ;

	public:
		ConfigParser( const ConfigParser &other ) ;
		ConfigParser& operator=( const ConfigParser &other ) ;
		~ConfigParser();
		
		static std::vector<ServerConfig> parse( const std::string &filePath ) ;

} ;

#endif