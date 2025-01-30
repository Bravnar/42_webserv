#ifndef CONFIGMANAGER_HPP
# define CONFIGMANAGER_HPP

# include <string>
# include <vector>
# include <iostream>
# include "ServerConfig.hpp"
# include "ConfigParser.hpp"

class ConfigManager {
	private:

		std::vector<ServerConfig>	_servers ;
		std::string					_confPath ;

		void						_parseConfig() ;

	public:

		ConfigManager( void ) ;
		ConfigManager( const std::string &filePath ) ;
		ConfigManager( const ConfigManager &other ) ;
		ConfigManager& operator=( const ConfigManager &other ) ;
		~ConfigManager( void ) ;

		const	std::vector<ServerConfig>&	getServers() const ;
		void	printConfig() const  ;

} ;

# endif 