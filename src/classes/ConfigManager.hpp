#ifndef CONFIGMANAGER_HPP
# define CONFIGMANAGER_HPP

# include <string>
# include <vector>
# include <iostream>
# include <iomanip>
# include <limits>
# include "ServerConfig.hpp"
# include "ConfigParser.hpp"
# include "../utils/Logger.hpp"

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
		int		getMinTimeout( void ) const ;

		void	printConfig() const  ;

} ;

# endif 