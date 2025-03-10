#include "./HttpRequest.hpp"
#include <stdexcept>

HttpRequest::HttpRequest():
	method_(""),
	url_(""),
	httpVersion_(""),
	_allBody(0),
	_boundary(""),
	_query("") {}

HttpRequest::HttpRequest(const std::string *buffer, std::string *bodyBuffer):
	method_(""),
	url_(""),
	httpVersion_(""),
	_allBody(0),
	_boundary(""),
	_query("")
{
	buildFromBuffer_(buffer, bodyBuffer);
}

HttpRequest::HttpRequest(const HttpRequest& copy):
	method_(copy.method_),
	url_(copy.url_),
	httpVersion_(copy.httpVersion_),
	headers_(copy.headers_),
	reqLine_(copy.reqLine_),
	_allBody(copy._allBody),
	_boundary(copy._boundary),
	_query(copy._query)
	{}

HttpRequest& HttpRequest::operator=(const HttpRequest& assign) {
	if (this != &assign){
		this->method_ = assign.method_;
		this->url_ = assign.url_;
		this->httpVersion_ = assign.httpVersion_;
		this->headers_ = assign.headers_;
		this->reqLine_ = assign.reqLine_;
		this->_query = assign._query;
		if (_allBody)
			delete _allBody;
		if (assign._allBody)
			this->_allBody = new std::string(assign._allBody->c_str(), assign._allBody->size());
		else
			_allBody = 0;
		this->_boundary = assign._boundary;
	}
	return *this;
}

HttpRequest::~HttpRequest() {}

bool checkFolder(const char* path) {
	struct stat s;

	if (!stat(path, &s)){
		if (!(s.st_mode & S_IFDIR))
			throw std::runtime_error("The path is not a directory");
		else
			return false;
	}
	return true;
}

void HttpRequest::parseRequestLine_(const std::string& line) {
	std::stringstream os(line);

	os >> method_ >> url_ >> httpVersion_;
	if (this->method_ != "GET" && this->method_ != "POST" && this->method_ != "DELETE") {
		#if LOGGER_DEBUG
			Logger::debug("request invalid method") << std::endl;
		#endif
		throw std::runtime_error(EXC_INVALID_RL);
	} else if (this->httpVersion_ != "HTTP/1.1") {
		#if LOGGER_DEBUG
			Logger::debug("request invalid http version") << std::endl;
		#endif
		throw std::runtime_error(EXC_INVALID_RL);
	}
	#if LOGGER_DEBUG < 0
		Logger::debug("removing query from url") << std::endl;
	#endif
	if (this->url_.find('?', 0) != std::string::npos) {
		this->_query = this->url_.substr(this->url_.find('?') + 1) ;
		this->url_ = this->url_.substr(0, this->url_.find('?', 0));
	}
	this->reqLine_ = this->method_ + " " + this->url_ + " " + this->httpVersion_;
}

void HttpRequest::buildBody(std::string location, std::string path) const{
	if (!_allBody)
		throw std::runtime_error("No Body set");
	if (checkFolder((location + "/" + path).c_str()))
		mkdir((location + "/" + path).c_str(), 0755);
	std::stringstream ss(_allBody->substr(0, _allBody->find("\r\n\r\n")));
	// std::stringstream ss(*_allBody);
	std::ofstream	file_dl;
	std::string line("");

	if (!_boundary.empty() && std::getline(ss, line)){
		line = line.substr(0, line.size() - 1);
		if (line == _boundary){
			if (std::getline(ss, line)){
				std::stringstream os(line);
				line.clear();
				while (line.find("name=") == line.npos)
					os >> line;
				if (line.find("fileToUpload") != line.npos){
					while (line.find("filename=") == line.npos)
						os >> line;
					for (std::string tmp = line; line.find("\"") == line.rfind("\""); line += " " + tmp)
						os >> tmp;
					line = line.substr(line.find("\"")+1, line.size() - line.find("\"") - 2);
					#if LOGGER_DEBUG
						Logger::debug(location + "/" + path + "/" + line) << std::endl;
					#endif
					const std::string fileName = location + "/" + path + "/" + line;
					file_dl.open(fileName.c_str(), std::ios::out | std::ios::binary);
					if (!file_dl.is_open())
						throw std::runtime_error("can\'t open/create the file");
					while(std::getline(ss, line)){
						line = line.substr(0, line.size() - 1);
						if (line.empty())
							break ;
					}
					size_t cursor_pos = _allBody->find("\r\n\r\n") + 4;

					size_t len = _allBody->find(_boundary, cursor_pos) - cursor_pos - 2;
					file_dl.write(_allBody->c_str() + cursor_pos, len);
					if (file_dl.fail()) {
						unlink(fileName.c_str());
						throw std::runtime_error(EXC_BODY_WRITE);
					}
					file_dl.close();
				}
			}
		}
		else throw (std::runtime_error(EXC_INVALID_BOUNDARY));
	}
}


int HttpRequest::buildFromBuffer_(const std::string *buffer, std::string *bodyBuffer){
	std::stringstream ss(*buffer);
	std::string line;

	size_t idx = 0;
	while (std::getline(ss, line))
	{
		line = line.substr(0, line.size() - 1);
		if (!idx)
			parseRequestLine_(line);
		else
		{
			if (line.empty())
				break;
			size_t sep = line.find(':', 0);
			if (sep != line.npos) {
				std::string key = line.substr(0, sep);
				std::string value = line.substr(sep + 2, line.size() - sep - 2);
				if (key == "Content-Type" && value.find("multipart/form-data") != value.npos)
					_boundary = "--" + value.substr(value.find("boundary=")+9, value.size());
				this->headers_[key] = value;
			}
		}
		idx++;
	}
	if (!bodyBuffer->empty()){
		// _allBody = new std::string(buffer->c_str() + cursor_pos, buffer->size() - cursor_pos);
		_allBody = bodyBuffer;
	}
	if (this->headers_.find(H_HOST) == this->headers_.end()) throw std::runtime_error(EXC_HEADER_NOHOST);
	return 0;
}

const std::string& HttpRequest::getMethod() const { return this->method_; }
const std::string* HttpRequest::getAllBody() const { return this->_allBody; }
const std::string& HttpRequest::getBoundary() const { return this->_boundary; }
const std::map<std::string, std::string>& HttpRequest::getHeaders() const { return this->headers_; }
const std::string& HttpRequest::getUrl() const { return this->url_; }
const std::string& HttpRequest::getHttpVersion() const { return this->httpVersion_; }
const std::string& HttpRequest::getReqLine() const { return this->reqLine_; }
const std::string& HttpRequest::getQuery() const { return this->_query; }
