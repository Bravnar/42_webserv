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
		std::vector<std::string>	_serverKeys ;
		std::vector<std::string>	_routeKeys ; 

	public:
		ConfigParser( const ConfigParser &other ) ;
		ConfigParser& operator=( const ConfigParser &other ) ;
		~ConfigParser();
		
		static std::vector<ServerConfig> parse( const std::string &filePath ) ;

} ;

#endif