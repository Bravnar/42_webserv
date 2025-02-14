#include "./HttpRequest.hpp"

HttpRequest::HttpRequest():
	method_(""),
	url_(""),
	httpVersion_(""),
	body_(0) {}

HttpRequest::HttpRequest(const std::string *buffer) {
	this->body_ = 0;
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
		size_t pos = line.find_first_of(' ', old_pos);
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
	if (this->method_ != "GET" && this->method_ != "POST") {
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
	if (this->url_.find_first_of('?', 0) != std::string::npos) {
		this->url_ = this->url_.substr(0, this->url_.find_first_of('?', 0));
	}
	this->reqLine_ = this->method_ + " " + this->url_ + " " + this->httpVersion_;
}

int HttpRequest::buildFromBuffer_(const std::string *buffer) {
	std::stringstream ss(*buffer);
	std::string line;
	bool isBody = false;

	size_t idx = 0;
	while (!isBody && std::getline(ss, line)) {
		line = line.substr(0, line.size() - 1);
		if (!idx) {
			parseRequestLine_(line);
		} else {
			if (line.empty()) {
				isBody = true;
				continue;
			}
			size_t sep = line.find_first_of(':', 0);
			if (sep != line.npos) {
				std::string key = line.substr(0, sep);
				std::string value = line.substr(sep + 2, line.size() - sep - 2);
				this->headers_[key] = value;
			}
		}
		idx++;
	}
	if (isBody) {
		std::map<std::string, std::string>::iterator it_contentlen = this->headers_.find(H_CONTENT_LENGTH);
		if (it_contentlen != this->headers_.end()) {
			long long bodySize = Convert::ToInt(this->headers_[H_CONTENT_LENGTH]);
			if (bodySize) {
				if (bodySize < 0) { throw std::runtime_error(EXC_BODY_NEG_SIZE); }
				const char *bodySep = std::strstr(buffer->data(), "\r\n\r\n");
				if (!bodySep) { throw std::runtime_error(EXC_BODY_NOLIMITER); }
				else {
					this->body_ = reinterpret_cast<const unsigned char *>(bodySep + 2);
					#if LOGGER_DEBUG
						Logger::debug("data: ") << this->getStringBody() << std::endl;
					#endif
				}
			}
		}
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