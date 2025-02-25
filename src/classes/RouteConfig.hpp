#ifndef ROUTECONFIG_HPP
# define ROUTECONFIG_HPP

# include <string>
# include <vector>
# include <utility>
# include <iostream>

class RouteConfig
{
	private:
		std::string						_path ; //from location line
		std::string						_locationRoot ; //from root
		std::vector<std::string>		_methods ; // 
		bool							_directoryListing ; //on/off
		bool							_acceptUploads ; // yes/no

		std::pair<std::string, std::string>	_cgi ;
		// std::string						_cgiExt ;

		std::string						_return ;
		std::string						_uploadPath ;
		std::string						_finalPath ;
		std::string						_index ; //TODO: add to config map !

	public:
		RouteConfig( );
		RouteConfig( const RouteConfig &other ) ;
		RouteConfig& operator=( const RouteConfig &other ) ;
		~RouteConfig( );

		void							setPath( const std::string &path ) ;
		void							setLocationRoot( const std::string &root ) ;
		void							addMethod( const std::string &path ) ;
		void							setDirectoryListing( bool io ) ;
		void							setAcceptUploads( bool io ) ;

		void							setCgi( const std::string &path, const std::string &ext ) ;
		// void							setCgiExt( const std::string &ext ) ;

		void							setReturn( const std::string &path ) ;
		void							setUploadPath( const std::string &path ) ;
		void							setFinalPath( const std::string &path ) ;
		void							setIndex( const std::string &path ) ;

		const std::string				&getPath( ) const ;
		const std::string				&getLocationRoot( ) const ;
		const std::vector<std::string>	&getMethods( ) const ;
		bool							isDirectoryListingEnabled( ) const ;
		bool							isUploadAccepted( ) const ;

		const std::pair<std::string, std::string>	&getCgi( ) const ;
		// const std::string				&getCgiExt( ) const ;

		const std::string				&getReturn( ) const ;
		const std::string				&getUploadPath( ) const ;
		const std::string				&getFinalPath( ) const ;
		const std::string				&getIndex( ) const ;
};

#endif