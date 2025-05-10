#pragma once

#include <string>
#include <variant>
#include <filesystem>

struct Context {
	enum class State {none, token, refresh, access};
	bool verbose, debug, confirm, help;
	int retry;
	State state;
	std::filesystem::path home;
	std::string hint, log;
	using options = std::variant<std::monostate, int*, std::string*>;
	bool stop();
	Context();
	~Context();
	void parse(size_t, char**);
	friend std::ostream& operator<<(std::ostream& oss, const Context& context);
	private:
	bool set(options&, const char*);
};
