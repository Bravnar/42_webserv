#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include <string>
# include <map>
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
		//main function that will take care of the parsing and populating the map
		static std::map<std::string, std::map<std::string, std::string> >
		parse( const std::string &filePath ) ;

	// nested classes for custom exceptions!
} ;

#endif