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

std::string Logger::formatLog(const std::string& msg) {
	std::string output = msg;
	std::size_t pos = 0;

	while ((pos = output.find("http://", pos)) != std::string::npos) {
		std::size_t end = output.find_first_of(" \t\n", pos);
		if (end == std::string::npos) {
			end = output.length();
		}
		std::string url = output.substr(pos, end - pos);
		std::string colored_url = std::string(C_CYAN) + C_UNDERLINE + url + C_RESET;
		output.replace(pos, url.length(), colored_url);
		pos += colored_url.length();
	}

	pos = 0;
	while ((pos = output.find("https://", pos)) != std::string::npos) {
		std::size_t end = output.find_first_of(" \t\n", pos);
		if (end == std::string::npos) {
			end = output.length();
		}
		std::string url = output.substr(pos, end - pos);
		std::string colored_url = std::string(C_CYAN) + C_UNDERLINE + url + C_RESET;
		output.replace(pos, url.length(), colored_url);
		pos += colored_url.length();
	}
	return output;
}

void Logger::baseLog(e_logger_lvl lvl, const std::string& msg) {
	std::ostream& stream = lvl <= 2 ? std::cerr : std::cout;

	stream << "[" << getCurrentDateTime() << "] ";
	switch(lvl) {
		case LFATAL:
			stream << C_RED << C_B_WHITE << "[FATAL]";
			break;
		case LERROR:
			stream << C_RED << "[ERROR]";
			break;
		case LWARNING:
			stream << C_YELLOW << "[WARNING]";
			break;
		case LINFO:
			stream << C_CYAN << "[INFO]";
			break;
		case LDEBUG:
			stream << C_PINK << "[DEBUG]";
			break;
	}
	stream << C_RESET << " " << formatLog(msg) << std::endl;
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