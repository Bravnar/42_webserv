#include "../utils/ReplaceAll.hpp"

std::string ReplaceAll::replace(const std::string& haystack, const std::string& needle, const std::string& replacement) {
	std::string output(haystack);
	if (needle.empty()) return haystack;
	size_t startPos = 0;

	while ((startPos = output.find(needle, startPos)) != std::string::npos) {
		output.replace(startPos, needle.length(), replacement);
		startPos += replacement.length();
	}
	return output;
}