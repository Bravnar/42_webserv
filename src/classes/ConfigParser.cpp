# include "ConfigParser.hpp"

ConfigParser::ConfigParser( void ) { } 

ConfigParser::ConfigParser( const ConfigParser &other ) { ( void ) other ; }

ConfigParser& ConfigParser::operator=( const ConfigParser &other ) {
	if ( this != &other ) ( void ) other ; 
	return *this ;
}

ConfigParser::~ConfigParser( void ) { }


std::vector<ServerConfig> ConfigParser::parse( const std::string &filePath ) {
	
	if ( filePath.substr( filePath.find_last_of(".") + 1) != "conf" ) {
		throw std::runtime_error( "Error: Invalid config extension, expected .conf" ) ;
	} std::ifstream	file(filePath) ;
	if ( !file ) {
		throw std::runtime_error( "Error: Could not open config file" ) ;
	} std::string	line ;
	while (std::getline( file, line )) {
		std::cout << "Reading line: " << line << std::endl ;
	}
	return std::vector<ServerConfig>() ;
}
