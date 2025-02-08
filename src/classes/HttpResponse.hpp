#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <vector>
# include <map>
# include <string>
# include "../utils/Convert.hpp"
# include "../utils/Logger.hpp"
# include <sys/socket.h>

class HttpResponse {
	private:
		const std::string version_;
		int status_;
		std::string status_msg_;
		std::map<std::string, std::string> headers_;
		const unsigned char *body_;
	public:
		// Canonical

		HttpResponse();
		HttpResponse(int status);
		HttpResponse(int status, const char *body, size_t bodySize, const std::string& url);
		HttpResponse(int status, const unsigned char *body, size_t bodySize, const std::string& url);
		HttpResponse(const HttpResponse&);
		HttpResponse& operator=(const HttpResponse&);
		~HttpResponse();

		// Member function

		const std::string str() const;
		void sendResp(int socket_fd) const;
		int getStatus() const;
		const std::string& getStatusMsg() const;
		const std::map<std::string, std::string>& getHeaders() const;

};

#endif