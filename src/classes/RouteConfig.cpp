# include "RouteConfig.hpp"

/* Default Constructor */
RouteConfig::RouteConfig( void ) :
_path(""),
_methods(),
_root(""),
_directoryListing(false),
_defaultFile(""),
_cgiPath(""),
_acceptUploads(false),
_uploadPath("") { }

/* Copy Constructor */
RouteConfig::RouteConfig( const RouteConfig &other ) :
_path(other._path),
_methods(other._methods),
_root(other._root),
_directoryListing(other._directoryListing),
_defaultFile(other._defaultFile),
_cgiPath(other._cgiPath),
_acceptUploads(other._acceptUploads),
_uploadPath(other._uploadPath) { }

/* Assignment Operator Overload */
RouteConfig& RouteConfig::operator=( const RouteConfig &other ) {
	if (this != &other) {
		_path = other._path ;
		_methods = other._methods ;
		_root = other._root ;
		_directoryListing = other._directoryListing ;
		_defaultFile = other._defaultFile;
		_cgiPath = other._cgiPath;
		_acceptUploads = other._acceptUploads ;
		_uploadPath = other._uploadPath ;
	}
	return *this ;
}
/* Destructor */
RouteConfig::~RouteConfig( void ) { }

/* SETTERS */
void	RouteConfig::setPath( const std::string &path ) { _path = path ; }
void	RouteConfig::addMethod( const std::string &path ) { _methods.push_back(path) ; }
void	RouteConfig::setRoot( const std::string &path ) { _root = path ; }
void	RouteConfig::setDirectoryListing( bool io ) { _directoryListing = io ; }
void	RouteConfig::setDefaultFile( const std::string &file ) { _defaultFile = file ; }
void	RouteConfig::setCgiPath( const std::string &cgiPath ) { _cgiPath = cgiPath ; }
void	RouteConfig::setAcceptUploads( bool io ) { _acceptUploads = io ; }
void	RouteConfig::setUploadPath( const std::string &path ) { _uploadPath = path ; }

/* GETTERS */
const std::string&	RouteConfig::getPath( ) const { return _path ; } 
const std::vector<std::string>&	RouteConfig::getMethods( ) const { return _methods ; } 
const std::string&	RouteConfig::getRoot( ) const { return _root ; } 
bool	RouteConfig::isDirectoryListingEnabled( ) const { return _directoryListing ; } 
const std::string&	RouteConfig::getDefaultFile( ) const { return _defaultFile ; } 
const std::string&	RouteConfig::getCgiPath( ) const { return _cgiPath ; } 
bool	RouteConfig::isUploadAccepted( ) const { return _acceptUploads ; } 
const std::string&	RouteConfig::getUploadPath( ) const { return _uploadPath ; } 