#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <iostream>
# include <string>
# include "./colors.h"
# include <ctime>
# include <sstream>
# include <iomanip>
# include <cstdio>

# ifndef LOGGER_DEBUG
#  define LOGGER_DEBUG 0
# endif

enum e_logger_lvl {
	LFATAL,
	LERROR,
	LWARNING,
	LINFO,
	LDEBUG
};

class Logger {
	private:
		Logger() {};
		Logger(const Logger&) {};
		Logger& operator=(const Logger&) {return *this;};
		~Logger() {};
		static std::ostream& baseLog(e_logger_lvl lvl, const std::string&);
		static std::string formatLog(const std::string&);

	public:
		static std::ostream& fatal(const std::string&);
		static std::ostream& error(const std::string&);
		static std::ostream& warning(const std::string&);
		static std::ostream& info(const std::string&);
		static std::ostream& debug(const std::string&);
};

#endif