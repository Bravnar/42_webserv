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
		template<typename T>
		static std::string ToString(const T& var) {
			std::ostringstream oss;

			oss << var;
			return oss.str();
		}
		template<typename T>
		static long long ToInt(const T& var) {
			std::stringstream ss(var);
			long long rslt;

			ss >> rslt;
			return rslt;
		}
		template<typename Y, typename T>
		static Y ToT(const T& var) {
			std::stringstream ss(var);
			Y rslt;

			ss >> rslt;
			return rslt;
		}
};

#endif