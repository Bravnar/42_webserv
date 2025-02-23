#include "./HttpRequest.hpp"
#include <algorithm>
#include <cstddef>
#include <fstream>
#include <ios>
#include <sstream>
#include <string>

HttpRequest::HttpRequest():
	method_(""),
	url_(""),
	httpVersion_(""),
	body_(0) {}

HttpRequest::HttpRequest(const std::string *buffer) : body_(0){
	buildFromBuffer_(buffer);
}

HttpRequest::HttpRequest(const HttpRequest& copy):
	method_(copy.method_),
	url_(copy.url_),
	httpVersion_(copy.httpVersion_),
	headers_(copy.headers_),
	body_(copy.body_),
	reqLine_(copy.reqLine_) {}

HttpRequest& HttpRequest::operator=(const HttpRequest& assign) {
	if (this == &assign)
		return *this;
	this->method_ = assign.method_;
	this->url_ = assign.url_;
	this->httpVersion_ = assign.httpVersion_;
	this->headers_ = assign.headers_;
	this->body_ = assign.body_;
	this->reqLine_ = assign.reqLine_;
	return *this;
}

HttpRequest::~HttpRequest() {
}

void HttpRequest::parseRequestLine_(const std::string& line) {
	int iter = 0;
	size_t old_pos = 0;

	while (iter < 3) {
		size_t pos = line.find(' ', old_pos);
		if ((iter != 2 && pos == line.npos) || (iter == 2 && pos != line.npos))
			throw std::runtime_error(EXC_INVALID_RL);
		switch(iter) {
			case 0:
				this->method_ = line.substr(old_pos, pos);
				break;
			case 1:
				this->url_ = line.substr(old_pos, pos - old_pos);
				break;
			default:
				this->httpVersion_ = line.substr(old_pos, line.size() - old_pos);
				break;
		}
		old_pos = pos + 1;
		iter++;
	}
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
	// TODO: CGI -> Currently I ignore the ? query from the url; otherwhise it would get me a 404 page because webserv would look for a file called "file?req"
	// Instead, you can store it in a new response parameter string/vector and make a getter for it while still removing it from url, so the server will still ignore this when making a query to a non-cgi location
	if (this->url_.find('?', 0) != std::string::npos) {
		this->url_ = this->url_.substr(0, this->url_.find('?', 0));
	}
	this->reqLine_ = this->method_ + " " + this->url_ + " " + this->httpVersion_;
}

int HttpRequest::buildFromBuffer_(const std::string *buffer) {
	std::stringstream ss(*buffer);
	std::string line;
	std::string boundary;
	std::ofstream	file_dl;

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
					boundary = "--" + value.substr(value.find("boundary=")+9, value.size());
				this->headers_[key] = value;
			}
		}
		idx++;
	}
	if (!boundary.empty() && std::getline(ss, line)){
		line = line.substr(0, line.size() - 1);
		if (this->headers_.find(H_CONTENT_LENGTH) != this->headers_.end()){
			long long bodySize = Convert::ToInt(this->headers_[H_CONTENT_LENGTH]);
			if (bodySize < 0) { throw std::runtime_error(EXC_BODY_NEG_SIZE); }
		}
		if (line == boundary){
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
					line = PATH_TO_DOWNLOAD + line.substr(line.find("\"")+1, line.size() - line.find("\"") - 2);
					#if LOGGER_DEBUG
						Logger::debug(line) << std::endl;
					#endif
					// file_dl.open(line.c_str(), std::ios::out | std::ios::binary);
					file_dl.open(line.c_str());
					if (!file_dl.is_open())
						throw std::runtime_error("can\'t open/create the file");
					while(std::getline(ss, line)){
						line = line.substr(0, line.size() - 1);
						if (line.empty())
							break ;
					}
					std::streamoff cursor_pos = ss.tellg();
					size_t len = buffer->find(boundary, cursor_pos) - cursor_pos - 2;
					std::string data = buffer->substr(cursor_pos, len);
					file_dl.write(data.c_str(), data.size());
					file_dl.close();
					data.clear();
				}
			}
		}
		else throw (std::runtime_error(EXC_INVALID_BOUNDARY));
	}
	if (this->headers_.find(H_HOST) == this->headers_.end()) { throw std::runtime_error(EXC_HEADER_NOHOST); }
	return 0;
}

const std::string& HttpRequest::getMethod() const { return this->method_; }
const std::map<std::string, std::string>& HttpRequest::getHeaders() const { return this->headers_; }
const unsigned char * HttpRequest::getBody() const { return this->body_; }
const std::string HttpRequest::getStringBody() const {
	if (this->body_) return std::string(reinterpret_cast<const char *>(this->body_), Convert::ToInt(this->headers_.at(H_CONTENT_LENGTH)));
	return "";
}
const std::string& HttpRequest::getUrl() const { return this->url_; }
const std::string& HttpRequest::getHttpVersion() const { return this->httpVersion_; }
const std::string& HttpRequest::getReqLine() const { return this->reqLine_; }
