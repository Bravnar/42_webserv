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
	} else {
        contentType = "application/octet-stream";
    }
    return contentType;
}

// TODO: use default errors when possible
HttpResponse::HttpResponse(int status): version_("HTTP/1.1"), status_(status), body_(0) {
	switch (status) {
		case 200:
			this->status_msg_ = "OK";
			break;
		case 400:
			this->status_msg_ = "Bad Request";
			break;
	}
	this->headers_.insert(std::make_pair("Date", getHttpDate()));
	this->headers_.insert(std::make_pair("Server", "SIR Webserver/1.0"));
}

HttpResponse::HttpResponse(int status, const char *body, size_t bodySize, const std::string& url): version_("HTTP/1.1"), status_(status), body_(reinterpret_cast<const unsigned char *>(body)) {
	switch (status) {
		case 200:
			this->status_msg_ = "OK";
			break;
		case 400:
			this->status_msg_ = "Bad Request";
			break;
		case 404:
			this->status_msg_ = "Not Found";
	}
	this->headers_.insert(std::make_pair("Date", getHttpDate()));
	this->headers_.insert(std::make_pair("Server", "SIR Webserver/1.0"));
	this->headers_.insert(std::make_pair("Content-Length", Convert::ToString(bodySize)));
	this->headers_.insert(std::make_pair("Content-Type", getType(url)));
}

HttpResponse::HttpResponse(int status, const unsigned char *body, size_t bodySize, const std::string& url): version_("HTTP/1.1"), status_(status), body_(body) {
	switch (status) {
		case 200:
			this->status_msg_ = "OK";
			break;
		case 400:
			this->status_msg_ = "Bad Request";
			break;
		case 404:
			this->status_msg_ = "Not Found";
	}
	this->headers_.insert(std::make_pair("Date", getHttpDate()));
	this->headers_.insert(std::make_pair("Server", "SIR Webserver/1.0"));
	this->headers_.insert(std::make_pair("Content-Length", Convert::ToString(bodySize)));
	this->headers_.insert(std::make_pair("Content-Type", getType(url)));
}

HttpResponse::HttpResponse(const HttpResponse& copy): version_(copy.version_), status_(copy.status_), status_msg_(copy.status_msg_), headers_(copy.headers_), body_(copy.body_) {
}

HttpResponse& HttpResponse::operator=(const HttpResponse& assign) {
	if (this == &assign)
		return *this;
	this->version_ = assign.version_;
	this->status_ = assign.status_;
	this->status_msg_ = assign.status_msg_;
	this->headers_ = assign.headers_;
	this->body_ = assign.body_;
	return *this;
}

HttpResponse::~HttpResponse() {

}

const std::string HttpResponse::str() const {
	std::string resp;
	std::ostringstream oss;

	oss	<< this->version_ << " " << this->status_ << " " << this->status_msg_ << "\r\n";
	for(std::map<std::string, std::string>::const_iterator it = this->headers_.begin(); it != this->headers_.end(); it++) {
		oss << it->first << ": " << it->second << "\r\n";
	}
	oss << "\r\n";
	resp = oss.str();
	if (this->body_)
		resp.append(reinterpret_cast<const char*>(this->body_), Convert::ToInt(this->headers_.at("Content-Length")));
	return resp;
}

#include <unistd.h>

void HttpResponse::sendResp(int socket_fd) const {
	ssize_t sendBytes;

	if ((sendBytes = send(socket_fd, this->str().data(), this->str().size(), 0)) > 0) {
		Logger::info("sent ") << sendBytes << " bytes" << std::endl;
	} else if (sendBytes < 0) {
		Logger::fatal("Error on sending data") << std::endl;
	}
	else {
		Logger::error("Nothing sent") << std::endl;
	}
}