
#ifndef CONFIGMANAGER_HPP
# define CONFIGMANAGER_HPP

# include <string>
# include <unordered_map>
# include <iostream>
# include "ConfigParser.hpp"

class ConfigManager {
	private:

		std::unordered_map<std::string, std::string>	_serverConf ;
		std::string										_confPath ;

		void											_parseConfig();

		ConfigManager( void ) ;
	public:

		ConfigManager( const std::string &filePath ) ;
		ConfigManager( const ConfigManager &other ) ;
		ConfigManager& operator=( const ConfigManager &other ) ;
		virtual ~ConfigManager( void ) ;

		std::string getConfig( const std::string& key ) const ;
		void		printConfig() const ;

} ;

# endif 