#include "./Convert.hpp"

std::string Convert::ToString(int i) {
	std::ostringstream oss;

	oss << i;
	return oss.str();
}