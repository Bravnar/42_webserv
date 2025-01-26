# include "ConfigParser.hpp"

ConfigParser::ConfigParser( void ) { } 

ConfigParser::ConfigParser( const ConfigParser &other ) { ( void ) other ; }

ConfigParser& ConfigParser::operator=( const ConfigParser &other ) {
	if ( this != &other ) ( void ) other ; 
	return *this ;
}

ConfigParser::~ConfigParser( void ) { }

