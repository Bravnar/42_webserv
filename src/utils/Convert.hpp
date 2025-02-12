#ifndef CONVERT_HPP
# define CONVERT_HPP

# include <sstream>
# include <string>

class Convert {
	private:
		Convert() {};
		Convert(const Convert&) {};
		Convert& operator=(const Convert&) {return *this;};
		~Convert() {};
	public:
		static std::string ToString(unsigned long long i);
		static long long ToInt(const std::string&);
};

#endif