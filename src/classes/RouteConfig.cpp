# include "RouteConfig.hpp"

//TODO: Remove ServerIndex
/* Default Constructor */
RouteConfig::RouteConfig( void ) :
_path(""),
_locationRoot(""),
_methods(),
_directoryListing(false),
_acceptUploads(false),
_cgi(),
// _cgiExt(""),
_return(""), 
_uploadPath("/uploads"),
_finalPath(""),
_index("index.html") { }

/* Copy Constructor */
RouteConfig::RouteConfig( const RouteConfig &other ) :
_path(other._path),
_locationRoot(other._locationRoot),
_methods(other._methods),
_directoryListing(other._directoryListing),
_acceptUploads(other._acceptUploads),
_cgi(other._cgi),
// _cgiExt(other._cgiExt),
_return(other._return),
_uploadPath(other._uploadPath),
_finalPath(other._finalPath),
_index(other._index) { }

/* Assignment Operator Overload */
RouteConfig& RouteConfig::operator=( const RouteConfig &other ) {
	if (this != &other) {
		_path = other._path ;
		_locationRoot = other._locationRoot ;
		_methods = other._methods ;
		_directoryListing = other._directoryListing ;
		_acceptUploads = other._acceptUploads ;
		_cgi = other._cgi ;
		// _cgiExt = other._cgiExt ;
		_return = other._return ;
		_uploadPath = other._uploadPath ;
		_finalPath = other._finalPath ;
		_index = other._index ; 
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

void	RouteConfig::setCgi( const std::string &path, const std::string &ext ) { _cgi.first = path ; _cgi.second = ext ; }
// void	RouteConfig::setCgiExt( const std::string &ext ) { _cgiExt = ext ; }

void	RouteConfig::setReturn( const std::string &path ) { _return = path ; }
void	RouteConfig::setUploadPath( const std::string &path ) { _uploadPath = path ; }
void	RouteConfig::setFinalPath( const std::string &path ) { _finalPath = path + "/" ; } 
void	RouteConfig::setIndex( const std::string &path ) { _index = path ; }

/* GETTERS */
const std::string&							RouteConfig::getPath( ) const { return _path ; } 
const std::string&							RouteConfig::getLocationRoot( ) const { return _locationRoot ; } 
const std::vector<std::string>&				RouteConfig::getMethods( ) const { return _methods ; } 
bool										RouteConfig::isDirectoryListingEnabled( ) const { return _directoryListing ; } 
bool										RouteConfig::isUploadAccepted( ) const { return _acceptUploads ; } 

const std::pair<std::string, std::string>&	RouteConfig::getCgi( ) const { return _cgi ; }
// const std::string&	RouteConfig::getCgiExt( ) const { return _cgiExt ; } 

const std::string&							RouteConfig::getReturn( ) const { return _return ; }
const std::string&							RouteConfig::getUploadPath( ) const { return _uploadPath ; } 
const std::string&							RouteConfig::getFinalPath( ) const { return _finalPath ; }
const std::string&							RouteConfig::getIndex( ) const { return _index ; }