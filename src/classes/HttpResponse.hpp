#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <vector>
# include <map>
# include <string>
# include "../utils/Convert.hpp"
# include "../utils/Logger.hpp"
# include <sys/socket.h>
# include <fstream>
# include "./ServerManager.hpp"

# define EXC_FILE_NOT_FOUND(file) file + " not found"
# define EXC_SEND_ERROR "Error on sending data to socket"

class HttpResponse {
	private:
		const std::string version_;
		int status_;
		std::string status_msg_;
		std::map<std::string, std::string> headers_;
		std::string fileName_;
	public:
		// Canonical

		HttpResponse();
		HttpResponse(int status);
		HttpResponse(int status, const std::string& fileName, const std::string& url);
		HttpResponse(const HttpResponse&);
		HttpResponse& operator=(const HttpResponse&);
		~HttpResponse();

		// Member function

		const std::string str() const;
		void sendResp(int socket_fd);
		int getStatus() const;
		const std::string& getStatusMsg() const;
		const std::map<std::string, std::string>& getHeaders() const;

};

#endif