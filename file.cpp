#include <iostream>
#include "file.hpp"

using namespace std;

File::File(string&& file): name(file), path(file) {
	open(path, ios::ate | ios::in);
}

iostream& File::operator>>(string& content)
{
	if (is_open()) close();
	open(path, ios::ate | ios::in);
	if (!*this) return *this;
	content.resize(tellg());
	seekg(0);
	read(content.data(), content.size());
	close();
	return *this;
}

iostream& File::operator<<(const string& content)
{
	if (!std::filesystem::exists(path.parent_path()))
		filesystem::create_directories(path.parent_path());
	if (is_open()) close();
	open(path, ios::trunc | ios::out);
	write(content.data(), content.size());
	close();
	return *this;
}

string File::getName() const
{
	return absolute(path);
}
