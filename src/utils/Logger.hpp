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
		static void baseLog(e_logger_lvl lvl, const std::string& msg);

	public:
		static void fatal(const std::string&);
		static void error(const std::string&);
		static void warning(const std::string&);
		static void info(const std::string&);
		static void debug(const std::string&);
};

#endif