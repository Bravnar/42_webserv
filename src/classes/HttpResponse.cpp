#include "./HttpResponse.hpp"

static std::string getHttpDate() {
	std::time_t now = std::time(0);
	std::tm *tm = std::gmtime(&now);
	char buf[30];
	std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", tm);
	return std::string(buf);
}

static bool endsWith(const std::string& url, const std::string& end) {
	if (url.length() >= end.length()) {
		return (!url.compare(url.length() - end.length(), end.length(), end));
	}
	return false;
}

const std::string HttpResponse::getType(const std::string& url) {
	if (endsWith(url, ".html")) { return "text/html"; }
	else if (endsWith(url, ".css")) { return "text/css"; }
	else if (endsWith(url, ".js")) { return "application/javascript"; }
	else if (endsWith(url, ".json")) { return "application/json"; }
	else if (endsWith(url, ".xml")) { return "application/xml"; }
	else if (endsWith(url, ".jpg"))  { return "image/jpeg"; }
	else if (endsWith(url, ".jpeg")) { return "image/jpeg"; }
	else if (endsWith(url, ".png")) { return "image/png"; }
	else if (endsWith(url, ".gif")) { return "image/gif"; }
	else if (endsWith(url, ".txt")) { return "text/plain"; }
	else if (endsWith(url, ".svg")) { return "image/svg+xml"; }
	else if (endsWith(url, ".ico")) { return "image/x-icon"; }
	else { return("application/octet-stream"); }
}

const std::string HttpResponse::checkStatus(int status) {
	switch (status) {
		case 100: return "Continue";
		case 200: return "OK";
		case 201: return "Created";
		case 301: return "Moved Permantly";
		case 400: return "Bad Request";
		case 405: return "Method Not Allowed";
		case 408: return "Request Timeout";
		case 404: return "Not Found";
		case 500: return "Internal Server Error";
		case 503: return "Service Unavailable";
		default: return "Unknown";
	}
}

HttpResponse::HttpResponse():
	version_("HTTP/1.1"),
	url_(0) {
		this->headers_[H_DATE] = getHttpDate();
		this->headers_[H_SERVER] = DF_H_SERVER;
		this->headers_[H_CONNECTION] = "close";
}

HttpResponse::HttpResponse(const HttpRequest& httpRequest):
	version_("HTTP/1.1"),
	status_(200),
	status_msg_(checkStatus(status_)),
	url_(0) {
		this->headers_[H_DATE] = getHttpDate();
		this->headers_[H_SERVER] = DF_H_SERVER;
		this->url_ = &httpRequest.getUrl();
		const std::map<std::string, std::string>& headers = httpRequest.getHeaders();
		if ((this->status_ >= 200 && this->status_ < 300)
			&& (headers.find(H_CONNECTION) == headers.end() || headers.at(H_CONNECTION) != "close"))
				this->headers_[H_CONNECTION] = "keep-alive";
		else
			this->headers_[H_CONNECTION] = "close";
}

HttpResponse::HttpResponse(const HttpRequest& httpRequest, int errorPage):
	version_("HTTP/1.1"),
	status_(errorPage),
	status_msg_(checkStatus(status_)),
	url_(0) {
		this->headers_[H_DATE] = getHttpDate();
		this->headers_[H_SERVER] = DF_H_SERVER;
		const std::map<std::string, std::string>& headers = httpRequest.getHeaders();
		if ((this->status_ >= 200 && this->status_ < 300)
			&& (headers.find(H_CONNECTION) == headers.end() || headers.at(H_CONNECTION) != "close"))
				this->headers_[H_CONNECTION] = "keep-alive";
		else
			this->headers_[H_CONNECTION] = "close";
}

HttpResponse::HttpResponse(const HttpRequest& httpRequest, const RouteConfig& routeConf):
	version_("HTTP/1.1"),
	status_(routeConf.getReturn().empty() ? 301 : 302),
	status_msg_(checkStatus(status_)),
	url_(0) {
		this->headers_[H_DATE] = getHttpDate();
		this->headers_[H_SERVER] = DF_H_SERVER;
		const std::map<std::string, std::string>& headers = httpRequest.getHeaders();
		if (headers.find(H_CONNECTION) == headers.end() || headers.at(H_CONNECTION) != "close")
			this->headers_[H_CONNECTION] = "keep-alive";
		else
			this->headers_[H_CONNECTION] = "close";
		if (status_ == 301)
			this->headers_[H_LOCATION] = "http://" + httpRequest.getHeaders().at(H_HOST) + routeConf.getPath() + "/";
		else
			this->headers_[H_LOCATION] = routeConf.getReturn();
}

HttpResponse::HttpResponse(const HttpResponse& copy):
	version_(copy.version_),
	status_(copy.status_),
	status_msg_(copy.status_msg_),
	headers_(copy.headers_),
	url_(copy.url_) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& assign) {
	if (this == &assign)
		return *this;
	this->status_ = assign.status_;
	this->status_msg_ = assign.status_msg_;
	this->headers_ = assign.headers_;
	this->url_ = assign.url_;
	return *this;
}

HttpResponse::~HttpResponse() {}

const std::string HttpResponse::str() const {
	std::ostringstream oss;

	oss	<< this->version_ << " " << this->status_ << " " << this->status_msg_ << "\r\n";
	for(std::map<std::string, std::string>::const_iterator it = this->headers_.begin(); it != this->headers_.end(); it++) {
		oss << it->first << ": " << it->second << "\r\n";
	}
	oss << "\r\n";
	return oss.str();
}

int HttpResponse::getStatus() const { return this->status_; }
void HttpResponse::setStatus(int status) { this->status_ = status; this->status_msg_ = checkStatus(status); }
const std::string& HttpResponse::getStatusMsg() const { return this->status_msg_; }
std::map<std::string, std::string>& HttpResponse::getHeaders() { return this->headers_; }
const std::string& HttpResponse::getVersion() const { return this->version_; }
const std::string HttpResponse::getResLine() const { return "Response " + Convert::ToString(this->status_) + " " + this->status_msg_; }
const std::string *HttpResponse::getUrl() const { return this->url_; }