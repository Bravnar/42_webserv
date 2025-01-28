# include "ConfigParser.hpp"

ConfigParser::ConfigParser( void ) { } 

ConfigParser::ConfigParser( const ConfigParser &other ) { ( void ) other ; }

ConfigParser& ConfigParser::operator=( const ConfigParser &other ) {
	if ( this != &other ) ( void ) other ; 
	return *this ;
}

ConfigParser::~ConfigParser( void ) { }

std::map<std::string, std::map<std::string, std::string> >
ConfigParser::parse( const std::string &filePath ) {
	(void) filePath ;
	std::cout << "Inside the parse function :)\n" ;
	return std::map<std::string, std::map<std::string, std::string> >() ;
}