#ifndef ROUTECONFIG_HPP
# define ROUTECONFIG_HPP

# include <string>
# include <vector>
# include <iostream>

class RouteConfig
{
	private:
		std::string						_path ; //from location line
		std::string						_locationRoot ; //from root
		std::vector<std::string>		_methods ; // 
		bool							_directoryListing ; //on/off
		bool							_acceptUploads ; // yes/no
		std::string						_uploadPath ;
		std::string						_finalPath ;

	public:
		RouteConfig();
		RouteConfig( const RouteConfig &other ) ;
		RouteConfig& operator=( const RouteConfig &other ) ;
		~RouteConfig();

		void							setPath( const std::string &path ) ;
		void							setLocationRoot( const std::string &root ) ;
		void							addMethod( const std::string &path ) ;
		void							setDirectoryListing( bool io ) ;
		void							setAcceptUploads( bool io ) ;
		void							setUploadPath( const std::string &path ) ;
		void							setFinalPath( const std::string &path ) ;

		const std::string				&getPath( ) const ;
		const std::string				&getLocationRoot( ) const ;
		const std::vector<std::string>	&getMethods( ) const ;
		bool							isDirectoryListingEnabled( ) const ;
		bool							isUploadAccepted( ) const ;
		const std::string				&getUploadPath( ) const ;
		const std::string				&getFinalPath( ) const ;
};

#endif