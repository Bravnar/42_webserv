#ifndef PARSEUTILS_HPP
# define PARSEUTILS_HPP

# include "../classes/ConfigParser.hpp"

/* Server Key Setters */

void	setHost( ServerConfig& server, const std::string &str ) ;
void	setPort( ServerConfig& server, const std::string &str ) ;
void	setServerNames( ServerConfig& server, const std::string &str) ;
void	setServerRoot( ServerConfig& server, const std::string &str ) ;
void	setError( ServerConfig& server, const std::string &str ) ;
void	setClientBodyLimit( ServerConfig& server, const std::string &str) ;
void	setMaxClients( ServerConfig& server, const std::string &str ) ;
void	setTimeout( ServerConfig& server, const std::string &str ) ;
void	setServerIndex( ServerConfig& server, const std::string &str ) ;

/* Route Key Setters */

void	setLocationRoot( RouteConfig& route, const std::string &str ) ;
void	setMethods( RouteConfig& route, const std::string &str ) ;

void	setCgi( RouteConfig& route, const std::string &str ) ; 
void	setCgiExtension( RouteConfig& route, const std::string &str ) ;

void	setReturn( RouteConfig& route, const std::string &str ) ;
void	setDirectoryListing( RouteConfig& route, const std::string &str ) ;
void	setAcceptUploads( RouteConfig& route, const std::string &str ) ;
void	setUploadPath( RouteConfig& route, const std::string &str ) ;
void	setLocationIndex( RouteConfig& route, const std::string &str ) ;

/* Helper functions */

std::string	trim( const std::string& str ) ;
std::string	replWhitespace( const std::string& str ) ;

#endif