#include "./Convert.hpp"

std::string Convert::ToString(unsigned long long i) {
	std::ostringstream oss;

	oss << i;
	return oss.str();
}

long long Convert::ToInt(const std::string& str) {
	std::stringstream ss(str);
	long long rslt;

	ss >> rslt;
	return rslt;
}