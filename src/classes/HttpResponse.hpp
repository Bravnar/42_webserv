#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <vector>
# include <map>
# include <string>
# include "../utils/Convert.hpp"
# include "../utils/Logger.hpp"
# include <sys/socket.h>
# include "./HttpRequest.hpp"
# include "./HttpInclude.hpp"

# define DF_H_SERVER "WebserverSIR/1.0"

# define EXC_FILE_NOT_FOUND(file) file + " not found"
# define EXC_SEND_ERROR "Error on sending data to socket"

class HttpResponse {
	private:
		const std::string version_;
		int status_;
		std::string status_msg_;
		std::map<std::string, std::string> headers_;
		const std::string *url_;
	public:
		// Canonical

		HttpResponse();
		HttpResponse(const HttpRequest& httpRequest);
		HttpResponse(int errorPage);
		HttpResponse(const HttpResponse&);
		HttpResponse& operator=(const HttpResponse&);
		~HttpResponse();

		// Member function

		// Retruns a new string of header
	
		const std::string str() const;

		// Getters
	
		int getStatus() const;
		void setStatus(int status);
		const std::string& getStatusMsg() const;
		std::map<std::string, std::string>& getHeaders();
		const std::string& getVersion() const;
		const std::string getResLine() const;
		const std::string *getUrl() const;

};

#endif