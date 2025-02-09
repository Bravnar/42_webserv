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

HttpResponse::HttpResponse(): version_("HTTP/1.1"), status_(0), body_(0) {
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

// TODO: use default errors when possible
HttpResponse::HttpResponse(int status): version_("HTTP/1.1"), status_(status), body_(0) {
	this->status_msg_ = checkStatus(status);
	this->headerBuffer_.insert(std::make_pair("Date", getHttpDate()));
	this->headerBuffer_.insert(std::make_pair("Server", "SIR Webserver/1.0"));
}

HttpResponse::HttpResponse(int status, const char *body, size_t bodySize, const std::string& url): version_("HTTP/1.1"), status_(status), body_(reinterpret_cast<const unsigned char *>(body)) {
	this->status_msg_ = checkStatus(status);
	this->headerBuffer_.insert(std::make_pair("Date", getHttpDate()));
	this->headerBuffer_.insert(std::make_pair("Server", "SIR Webserver/1.0"));
	this->headerBuffer_.insert(std::make_pair("Content-Length", Convert::ToString(bodySize)));
	this->headerBuffer_.insert(std::make_pair("Content-Type", getType(url)));
}

HttpResponse::HttpResponse(int status, const unsigned char *body, size_t bodySize, const std::string& url): version_("HTTP/1.1"), status_(status), body_(body) {
	this->status_msg_ = checkStatus(status);
	this->headerBuffer_.insert(std::make_pair("Date", getHttpDate()));
	this->headerBuffer_.insert(std::make_pair("Server", "SIR Webserver/1.0"));
	this->headerBuffer_.insert(std::make_pair("Content-Length", Convert::ToString(bodySize)));
	this->headerBuffer_.insert(std::make_pair("Content-Type", getType(url)));
}

HttpResponse::HttpResponse(const HttpResponse& copy): version_(copy.version_), status_(copy.status_), status_msg_(copy.status_msg_), headerBuffer_(copy.headerBuffer_), body_(copy.body_) {
}

HttpResponse& HttpResponse::operator=(const HttpResponse& assign) {
	if (this == &assign)
		return *this;
	this->status_ = assign.status_;
	this->status_msg_ = assign.status_msg_;
	this->headerBuffer_ = assign.headerBuffer_;
	this->body_ = assign.body_;
	return *this;
}

HttpResponse::~HttpResponse() {

}

const std::string HttpResponse::str() const {
	std::string resp;
	std::ostringstream oss;

	oss	<< this->version_ << " " << this->status_ << " " << this->status_msg_ << "\r\n";
	for(std::map<std::string, std::string>::const_iterator it = this->headerBuffer_.begin(); it != this->headerBuffer_.end(); it++) {
		oss << it->first << ": " << it->second << "\r\n";
	}
	if (this->body_)
		oss << "\r\n";
	resp = oss.str();
	if (this->body_)
		resp.append(reinterpret_cast<const char*>(this->body_), Convert::ToInt(this->headerBuffer_.at("Content-Length")));
	return resp;
}

void HttpResponse::sendResp(int socket_fd) const {
	if (send(socket_fd, this->str().data(), this->str().size(), 0) < 0) {
		Logger::fatal("Error on sending data") << std::endl;
	}
}

int HttpResponse::getStatus() const {
	return this->status_;
}

const std::string& HttpResponse::getStatusMsg() const {
	return this->status_msg_;
}

const std::map<std::string, std::string>& HttpResponse::getHeaders() const {
	return this->headerBuffer_;
}