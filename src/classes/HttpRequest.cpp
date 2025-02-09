#include "./HttpRequest.hpp"

int HttpRequest::parseRequestLine_(const std::string& line) {
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
				if (this->url_.at(0) != '/')
					this->url_ = "/" + this->url_;
				break;
			default:
				this->httpVersion_ = line.substr(old_pos, line.size() - old_pos);
				break;
		}
		old_pos = pos + 1;
		iter++;
	}
	if (this->method_ != "GET" && this->method_ != "POST") {
		Logger::debug("request invalid method") << std::endl;
		throw std::runtime_error(EXC_INVALID_RL);
	} else if (this->httpVersion_ != "HTTP/1.1") {
		Logger::debug("request invalid http version") << std::endl;
		throw std::runtime_error(EXC_INVALID_RL);
	}
	return 0;
}

int HttpRequest::parseBuffer_(const char *buffer) {
	this->buffer_str_ = new std::string(buffer);
	std::stringstream ss(*this->buffer_str_);
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
				this->headers_.insert(std::make_pair(key, value));
			}
		}
		idx++;
	}
	delete this->buffer_str_;
	this->buffer_str_ = 0;
	if (isBody) {
		std::map<std::string, std::string>::iterator it_contentlen = this->headers_.find(H_CONTENT_LENGTH);
		if (it_contentlen != this->headers_.end()) {
			long long bodySize = Convert::ToInt(this->headers_[H_CONTENT_LENGTH]);
			if (bodySize) {
				if (bodySize < 0) { throw std::runtime_error(EXC_BODY_NEG_SIZE); }
				this->body_ = new unsigned char[bodySize];
				const char *bodySep = std::strstr(buffer, "\r\n\r\n");
				if (!bodySep) { throw std::runtime_error(EXC_BODY_NOLIMITER); }
				else {
					const unsigned char *data = reinterpret_cast<const unsigned char *>(bodySep + 2);
					try { std::copy(data, data + bodySize, this->body_); }
					catch(const std::exception& e) { throw std::runtime_error(EXC_BODY_REQ_COPY); }
					Logger::debug("data: ") << this->getStringBody() << std::endl;	
				}
			}
		}
	}
	if (this->headers_.find(H_HOST) == this->headers_.end()) { throw std::runtime_error(EXC_HEADER_NOHOST); }
	return 0;
}

HttpRequest::HttpRequest():
	method_(""),
	url_(""),
	httpVersion_(""),
	body_(0),
	isValid_(false),
	buffer_str_(0) {}

HttpRequest::HttpRequest(const char *buffer): buffer_str_(0) {
	this->body_ = 0;
	parseBuffer_(buffer);
	this->isValid_ = true;
}

// TODO: deep copy
HttpRequest::HttpRequest(const HttpRequest& copy):
	method_(copy.method_),
	url_(copy.url_),
	httpVersion_(copy.httpVersion_),
	headers_(copy.headers_),
	body_(copy.body_),
	isValid_(copy.isValid_),
	buffer_str_(copy.buffer_str_) {}

// TODO: deep copy
HttpRequest& HttpRequest::operator=(const HttpRequest& assign) {
	if (this == &assign)
		return *this;
	this->method_ = assign.method_;
	this->url_ = assign.url_;
	this->httpVersion_ = assign.httpVersion_;
	this->headers_ = assign.headers_;
	this->body_ = assign.body_;
	this->isValid_ = assign.isValid_;
	this->buffer_str_ = assign.buffer_str_;
	return *this;
}

HttpRequest::~HttpRequest() {
	if (body_)
		delete[] body_;
	if (buffer_str_)
		delete this->buffer_str_;
}

const std::string& HttpRequest::getMethod() const {
	return this->method_;
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const {
	return this->headers_;
}

const unsigned char * HttpRequest::getBody() const {
	return this->body_;
}

const std::string HttpRequest::getStringBody() const {
	if (this->body_) {
		return std::string(reinterpret_cast<const char *>(this->body_), Convert::ToInt(this->headers_.at(H_CONTENT_LENGTH)));
	}
	return "";
}

bool HttpRequest::isValid() const {
	return this->isValid_;
}

const std::string& HttpRequest::getUrl() const {
	return this->url_;
}

const std::string& HttpRequest::getHttpVersion() const {
	return this->httpVersion_;
}
