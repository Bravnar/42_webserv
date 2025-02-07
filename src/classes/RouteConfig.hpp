#ifndef ROUTECONFIG_HPP
# define ROUTECONFIG_HPP

# include <string>
# include <vector>
# include <iostream>

class RouteConfig
{
	private:
		std::string						_path ;
		std::vector<std::string>		_methods ;
		std::string						_root ;
		bool							_directoryListing ; // yes/no
		// std::string						_defaultFile ;
		std::string						_cgiPath ;
		std::string						_redirect ; 
		bool							_acceptUploads ; // yes/no
		std::string						_uploadPath ;

	public:
		RouteConfig();
		RouteConfig( const RouteConfig &other ) ;
		RouteConfig& operator=( const RouteConfig &other ) ;
		~RouteConfig();

		void							setPath( const std::string &path ) ;
		void							addMethod( const std::string &path ) ;
		void							setRoot( const std::string &root ) ;
		void							setDirectoryListing( bool io ) ;
		// void							setDefaultFile( const std::string &file ) ;
		void							setCgiPath( const std::string &cgiPath ) ;
		void							setRedirect( const std::string &path ) ;
		void							setAcceptUploads( bool io ) ;
		void							setUploadPath( const std::string &path ) ;

		const std::string				&getPath( ) const ;
		const std::vector<std::string>	&getMethods( ) const ;
		const std::string				&getRoot( ) const ;
		bool							isDirectoryListingEnabled( ) const ;
		// const std::string				&getDefaultFile( ) const ;
		const std::string				&getCgiPath( ) const ;
		const std::string				&getRedirect( ) const ;
		bool							isUploadAccepted( ) const ;
		const std::string				&getUploadPath( ) const ;	
};

#endif