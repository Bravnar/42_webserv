#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "./../utils/Logger.hpp"
# include <vector>
# include <map>
# include <algorithm>
# include "./../utils/Convert.hpp"
# include <cstring>

# define EXC_INVALID_RL "invalid RequestLine"
# define EXC_BODY_NEG_SIZE "Negative body size"
# define EXC_BODY_NOLIMITER "No body delimiter"
# define EXC_BODY_REQ_COPY "Error on http request body copy"
# define EXC_HEADER_NOHOST "No host header"

# define H_HOST "Host"
# define H_USERAGENT "User-Agent"
# define H_ACCEPT "Accept"
# define H_ACCEPT_LANGUAGE "Accept-Language"
# define H_ACCEPT_ENCODING "Accept-Encoding"
# define H_CONNECTION "Connection"
# define H_CONTENT_TYPE "Content-Type"
# define H_CONTENT_LENGTH "Content-Length"
# define H_AUTHORIZATION "Authorization"
# define H_REFERER "Referer"
# define H_COOKIE "Cookie"
# define H_CACHE_CONTROL "Cache-Control"

class HttpRequest {
	private:
		int parseRequestLine_(const std::string&);
		int parseBuffer_(const char *buffer);
		std::string method_;
		std::string url_;
		std::string httpVersion_;
		std::map<std::string, std::string> headers_;
		unsigned char *body_;
		bool isValid_;
		std::string *body_buffer_;

	public:
		// canonical

		HttpRequest();
		HttpRequest(const char *buffer);
		HttpRequest(const HttpRequest&);
		HttpRequest& operator=(const HttpRequest&);
		~HttpRequest();

		// member functions
	
		const std::string& getMethod() const;
		const std::map<std::string, std::string>& getHeaders() const;
		const unsigned char *getBody() const;
		const std::string getStringBody() const;
		const std::string& getUrl() const;
		const std::string& getHttpVersion() const;
		bool isValid() const;
};

#endif