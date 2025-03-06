#ifndef REPLACEALL_HPP
# define REPLACEALL_HPP

# include <sstream>
# include <string>

class ReplaceAll {
	private:
		ReplaceAll() {};
		ReplaceAll(const ReplaceAll&) {};
		ReplaceAll& operator=(const ReplaceAll&) {return *this;};
		~ReplaceAll() {};
	public:
		static std::string replace(const std::string& haystack, const std::string& needle, const std::string& replacement);
};

#endif