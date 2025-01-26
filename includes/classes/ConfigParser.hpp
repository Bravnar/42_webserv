#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include <string>
# include <unordered_map>
# include <iostream>
# include <fstream>
# include <stdexcept>
# include <exception>

class ConfigParser
{
	private:
		ConfigParser();
		ConfigParser( const ConfigParser &other ) ;
		ConfigParser& operator=( const ConfigParser &other ) ;
		~ConfigParser();

	public:
		static std::unordered_map<std::string, std::string> parse( const std::string &filePath ) ;

	// nested classes for custom exceptions!
} ;

#endif