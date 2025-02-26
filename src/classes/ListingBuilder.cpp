#include "ListingBuilder.hpp"

ListingBuilder::ListingBuilder() {}
ListingBuilder::ListingBuilder(const ListingBuilder& copy) {(void)copy;}
ListingBuilder& ListingBuilder::operator=(const ListingBuilder& assign) {(void)assign; return *this;}
ListingBuilder::~ListingBuilder() {}

std::string ListingBuilder::buildBody(const std::string& url, const std::string& file) {
	std::string body;

	std::string parsedUrl = url.at(url.length() - 1) == '/' ? url.substr(0, url.length() - 1) : url;
	DIR *dir;
	dir = opendir(file.c_str());
	if (!dir) throw std::runtime_error(EXC_NO_DIR);
	body.append("<html><head><title>Index of " + parsedUrl + "</title></head><body><h1>Index of " + parsedUrl + "</h1><hr><pre>");
	struct dirent *entry;
	std::map<std::string, dirent *> files;
	while ((entry = readdir(dir))) {
		if (*entry->d_name == '.' && !*(entry->d_name + 1))
			continue;
		if (entry->d_type == DT_DIR)
			files[entry->d_name + std::string("/")] = entry;
		else
			files[entry->d_name] = entry;
	}
	for(std::map<std::string, dirent *>::const_iterator it = files.begin(); it != files.end(); it++) {
		body.append("<a href=\"" + parsedUrl + "/" + std::string(it->first) + "\">" + std::string(it->first) + "</a><br>");
	}
	body.append("</pre><hr></body></html>");
	return body;
}