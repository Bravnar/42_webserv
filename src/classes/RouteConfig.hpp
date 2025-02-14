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
		bool							_isCgi ;
		std::string						_return ;
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
		void							setIsCgi( bool io ) ;
		void							setReturn( const std::string &path ) ;
		void							setUploadPath( const std::string &path ) ;
		void							setFinalPath( const std::string &path ) ;

		const std::string				&getPath( ) const ;
		const std::string				&getLocationRoot( ) const ;
		const std::vector<std::string>	&getMethods( ) const ;
		bool							isDirectoryListingEnabled( ) const ;
		bool							isUploadAccepted( ) const ;
		bool							getIsCgi( ) const ;
		const std::string				&getReturn( ) const ;
		const std::string				&getUploadPath( ) const ;
		const std::string				&getFinalPath( ) const ;
};

#endif