#include <fstream>
#include <filesystem>

class File: public std::fstream {
	std::filesystem::path path;
	std::string name;
	public:
	File(std::string&&);
	std::iostream& operator>>(std::string&);
	std::iostream& operator<<(const std::string&);
	std::string getName() const;
};
