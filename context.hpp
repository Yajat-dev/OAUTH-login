#pragma once

#include <string>
#include <variant>
#include "code.hpp"

struct Context {
	bool verbose, debug, confirm;
	bool help;
	int server, client, port;
	Code crypto;
	std::string home, hint, code, refresh, secret;
	using options = std::variant<std::monostate, std::string*>;
	private:
	bool set(options&, const char*);
	std::string url_encode(const std::string&);
	public:
	bool getAccess();
	void getToken();
	Context();
	~Context();
	void parse(size_t, char**);
	friend std::ostream& operator<<(std::ostream& oss, const Context& context);
};
