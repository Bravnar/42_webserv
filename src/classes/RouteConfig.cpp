# include "RouteConfig.hpp"

/* Default Constructor */
RouteConfig::RouteConfig( void ) :
_path(""),
_locationRoot(""),
_methods(),
_directoryListing(false),
_acceptUploads(false),
_uploadPath("/uploads"),
_finalPath("") {
	// _methods.push_back("GET") ;
	// _methods.push_back("POST") ;
	// _methods.push_back("DELETE") ;
}

/* Copy Constructor */
RouteConfig::RouteConfig( const RouteConfig &other ) :
_path(other._path),
_locationRoot(other._locationRoot),
_methods(other._methods),
_directoryListing(other._directoryListing),
_acceptUploads(other._acceptUploads),
_uploadPath(other._uploadPath),
_finalPath(other._finalPath) { }

/* Assignment Operator Overload */
RouteConfig& RouteConfig::operator=( const RouteConfig &other ) {
	if (this != &other) {
		_path = other._path ;
		_locationRoot = other._locationRoot ;
		_methods = other._methods ;
		_directoryListing = other._directoryListing ;
		_acceptUploads = other._acceptUploads ;
		_uploadPath = other._uploadPath ;
		_finalPath = other._finalPath ;
	}
	return *this ;
}
/* Destructor */
RouteConfig::~RouteConfig( void ) { }

/* SETTERS */
void	RouteConfig::setPath( const std::string &path ) { _path = path ; }
void	RouteConfig::setLocationRoot( const std::string &path ) { _locationRoot = path ; }
void	RouteConfig::addMethod( const std::string &path ) { _methods.push_back(path) ; }
void	RouteConfig::setDirectoryListing( bool io ) { _directoryListing = io ; }
void	RouteConfig::setAcceptUploads( bool io ) { _acceptUploads = io ; }
void	RouteConfig::setUploadPath( const std::string &path ) { _uploadPath = path ; }
void	RouteConfig::setFinalPath( const std::string &path ) { _finalPath = path ; } 

/* GETTERS */
const std::string&	RouteConfig::getPath( ) const { return _path ; } 
const std::string&	RouteConfig::getLocationRoot( ) const { return _locationRoot ; } 
const std::vector<std::string>&	RouteConfig::getMethods( ) const { return _methods ; } 
bool	RouteConfig::isDirectoryListingEnabled( ) const { return _directoryListing ; } 
bool	RouteConfig::isUploadAccepted( ) const { return _acceptUploads ; } 
const std::string&	RouteConfig::getUploadPath( ) const { return _uploadPath ; } 
const std::string&	RouteConfig::getFinalPath( ) const { return _finalPath ; }