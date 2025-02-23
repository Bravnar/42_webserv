#ifndef LISTING_BUILDER_HPP
# define LISTING_BUILDER_HPP

# include <string>
# include <dirent.h>
# include <map>
# include <stdexcept>

# define EXC_NO_DIR "Fail on ListingBuilder"

class ListingBuilder {
	public:
		static std::string buildBody(const std::string& url, const std::string& file);
	private:
		ListingBuilder();
		ListingBuilder(const ListingBuilder&);
		ListingBuilder& operator=(const ListingBuilder&);
		~ListingBuilder();
};

#endif