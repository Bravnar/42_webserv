#ifndef ERRORBUILDER_HPP
# define ERRORBUILDER_HPP

# include "./HttpResponse.hpp"

class ErrorBuilder {
	public:
		static std::string buildBody(const HttpResponse&);
	private:
		ErrorBuilder();
		ErrorBuilder(const ErrorBuilder&);
		ErrorBuilder& operator=(const ErrorBuilder&);
		~ErrorBuilder();
};

#endif