#include "./HttpResponse.hpp"

static std::string getHttpDate() {
    std::time_t now = std::time(0);
    std::tm *tm = std::gmtime(&now);
    char buf[30];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", tm);
    return std::string(buf);
}

static std::string getType(const std::string& url) {
    std::string contentType;
    if (url.find(".html\0") != std::string::npos) {
        contentType = "text/html";
    } else if (url.find(".css\0") != std::string::npos) {
        contentType = "text/css";
    } else if (url.find(".js\0") != std::string::npos) {
        contentType = "application/javascript";
    } else if (url.find(".json\0") != std::string::npos) {
        contentType = "application/json";
    } else if (url.find(".xml\0") != std::string::npos) {
        contentType = "application/xml";
    } else if (url.find(".jpg\0") != std::string::npos || url.find(".jpeg\0") != std::string::npos) {
        contentType = "image/jpeg";
    } else if (url.find(".png\0") != std::string::npos) {
        contentType = "image/png";
    } else if (url.find(".gif\0") != std::string::npos) {
        contentType = "image/gif";
    } else if (url.find(".txt\0") != std::string::npos) {
        contentType = "text/plain";
    } else if (url.find(".svg\0") != std::string::npos) {
		contentType = "image/svg+xml";
	} else if (url.find(".ico\0") != std::string::npos) {
		contentType = "image/x-icon";
	} else {
        contentType = "application/octet-stream";
    }
    return contentType;
}

static std::string checkStatus(int status) {
	switch (status) {
		case 100: return "Continue";
		case 200: return "OK";
		case 201: return "Created";
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
	version_("HTTP/1.1") {
		this->headers_[H_DATE] = getHttpDate();
		this->headers_[H_SERVER] = DF_H_SERVER;
		this->headers_[H_CONNECTION] = "close";
}

HttpResponse::HttpResponse(const HttpRequest& httpRequest):
	version_("HTTP/1.1"),
	status_(200),
	status_msg_(checkStatus(status_)) {
		this->headers_[H_DATE] = getHttpDate();
		this->headers_[H_SERVER] = DF_H_SERVER;
		this->headers_[H_CONTENT_TYPE] = getType(httpRequest.getUrl());
		this->headers_[H_CONNECTION] = "close";
}

HttpResponse::HttpResponse(int errorPage):
	version_("HTTP/1.1"),
	status_(errorPage),
	status_msg_(checkStatus(status_)) {
		this->headers_[H_DATE] = getHttpDate();
		this->headers_[H_SERVER] = DF_H_SERVER;
		this->headers_[H_CONNECTION] = "close";
	(void)errorPage;
}

HttpResponse::HttpResponse(const HttpResponse& copy):
	version_(copy.version_),
	status_(copy.status_),
	status_msg_(copy.status_msg_),
	headers_(copy.headers_) {}

HttpResponse& HttpResponse::operator=(const HttpResponse& assign) {
	if (this == &assign)
		return *this;
	this->status_ = assign.status_;
	this->status_msg_ = assign.status_msg_;
	this->headers_ = assign.headers_;
	return *this;
}

HttpResponse::~HttpResponse() {}

const std::string HttpResponse::str() const {
	std::ostringstream oss;

	oss	<< this->version_ << " " << this->status_ << " " << this->status_msg_ << "\r\n";
	for(std::map<std::string, std::string>::const_iterator it = this->headers_.begin(); it != this->headers_.end(); it++) {
		oss << it->first << ": " << it->second << "\r\n";
	}
	return oss.str();
}

int HttpResponse::getStatus() const { return this->status_; }
void HttpResponse::setStatus(int status) { this->status_ = status; this->status_msg_ = checkStatus(status); }
const std::string& HttpResponse::getStatusMsg() const { return this->status_msg_; }
std::map<std::string, std::string>& HttpResponse::getHeaders() { return this->headers_; }
const std::string& HttpResponse::getVersion() const { return this->version_; }
const std::string HttpResponse::getResLine() const { return "Response " + Convert::ToString(this->status_) + " " + this->status_msg_; }