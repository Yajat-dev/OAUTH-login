#pragma once

#include <string>
#include <variant>
#include <filesystem>

struct Context {
	bool verbose, debug, confirm, help;
	int retry;
	std::filesystem::path home;
	std::string hint, log;
	using options = std::variant<std::monostate, int*, std::string*>;
	private:
	bool set(options&, const char*);
	public:
	bool stop();
	Context();
	void parse(size_t, char**);
	friend std::ostream& operator<<(std::ostream& oss, const Context& context);
};
