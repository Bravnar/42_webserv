#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "./../utils/Logger.hpp"
# include <vector>
# include <map>
# include <algorithm>
# include "./../utils/Convert.hpp"
# include <cstring>
# include "./HttpInclude.hpp"

# define EXC_INVALID_RL "invalid RequestLine"
# define EXC_BODY_NEG_SIZE "Negative body size"
# define EXC_BODY_NOLIMITER "No body delimiter"
# define EXC_BODY_REQ_COPY "Error on http request body copy"
# define EXC_HEADER_NOHOST "No host header"

class HttpRequest {
	private:
		int parseRequestLine_(const std::string&);
		int buildFromBuffer_(const char *reqBuffer);

		std::string method_;
		std::string url_;
		std::string httpVersion_;
		std::map<std::string, std::string> headers_;
		unsigned char *body_;
		std::string *body_buffer_;
		std::string reqLine_;

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
		const std::string& getReqLine() const;
};

#endif