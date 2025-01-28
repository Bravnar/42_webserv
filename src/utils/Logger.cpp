#include "./Logger.hpp"
#include <sys/time.h>

static std::string getCurrentDateTime() {
	struct timeval tv;
	gettimeofday(&tv, 0);

	time_t now = tv.tv_sec;
	struct tm *ltm = localtime(&now);

	char date_time[30];
	strftime(date_time, sizeof(date_time), "%Y-%m-%d %H:%M:%S", ltm);

	char current_time[40];
	snprintf(current_time, sizeof(current_time), "%s.%03ld", date_time, tv.tv_usec / 1000);

	return std::string(current_time);
}

void Logger::baseLog(e_logger_lvl lvl, const std::string& msg) {
	std::cout << "[" << getCurrentDateTime() << "] ";
	switch(lvl) {
		case LFATAL:
			std::cout << C_RED << C_B_WHITE << "[FATAL]";
			break;
		case LERROR:
			std::cout << C_RED << "[ERROR]";
			break;
		case LWARNING:
			std::cout << C_YELLOW << "[WARNING]";
			break;
		case LINFO:
			std::cout << C_CYAN << "[INFO]";
			break;
		case LDEBUG:
			std::cout << C_PINK << "[DEBUG]";
			break;
	}
	std::cout << C_RESET << " " << msg << std::endl;
}

void Logger::fatal(const std::string& str) {
	Logger::baseLog(LFATAL, str);
}

void Logger::error(const std::string& str) {
	Logger::baseLog(LERROR, str);
}

void Logger::warning(const std::string& str) {
	Logger::baseLog(LWARNING, str);
}

void Logger::info(const std::string& str) {
	Logger::baseLog(LINFO, str);
}

void Logger::debug(const std::string& str) {
	if (LOGGER_DEBUG)
		Logger::baseLog(LDEBUG, str);
}