#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "./../utils/Logger.hpp"
# include <vector>
# include <map>
# include <algorithm>
# include "./../utils/Convert.hpp"
# include <cstring>
# include "./HttpInclude.hpp"
# include <unistd.h>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

# define PATH_TO_DOWNLOAD "./www/uploads/"
# define EXC_INVALID_RL "invalid RequestLine"
# define EXC_INVALID_BOUNDARY "invalid Boundary"
# define EXC_BODY_NEG_SIZE "Negative body size"
# define EXC_BODY_NOLIMITER "No body delimiter"
# define EXC_HEADER_NOHOST "No host header"
# define EXC_BODY_WRITE "Error on body writting"

class HttpRequest {
	private:
		// Parse request line
		// @throws `EXC_INVALID_RL`
		void parseRequestLine_(const std::string&);
		int buildFromBuffer_(const std::string *, std::string *);

		std::string method_;
		std::string url_;
		std::string httpVersion_;
		std::map<std::string, std::string> headers_;
		std::string reqLine_;
		std::string *_allBody;
		std::string _boundary;

	public:
		// canonical

		HttpRequest();
		// @throws `EXC_INVALID_RL`
		HttpRequest(const std::string *, std::string *);
		HttpRequest(const HttpRequest&);
		HttpRequest& operator=(const HttpRequest&);
		~HttpRequest();

		// member functions

		// Getters

		const std::string& getMethod() const;
		const std::string* getAllBody() const;
		const std::string& getBoundary() const;
		const std::map<std::string, std::string>& getHeaders() const;
		const std::string& getUrl() const;
		const std::string& getHttpVersion() const;
		const std::string& getReqLine() const;
		void buildBody(std::string location, std::string path) const;
};

#endif
