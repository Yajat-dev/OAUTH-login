#pragma once

#include <string>
#include <variant>

struct Context {
	static bool verbose, debug, confirm;
	bool help;
	int server, client, port;
	std::string hint, code, refresh, token;
	using options = std::variant<std::monostate, std::string*>;
	private:
	bool set(options&, const char*);
	public:
	bool connect();
	std::string verifier(size_t);
	Context();
	~Context();
	void parse(size_t, char**);
	friend std::ostream& operator<<(std::ostream& oss, const Context& context);
};
