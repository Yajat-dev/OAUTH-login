#pragma once

#include <string>
#include <variant>
#include "code.hpp"

struct Context {
	bool verbose, debug, confirm, help;
	int server, client, port;
	Code crypto;
	std::string home, hint, code, refresh, secret;
	using options = std::variant<std::monostate, std::string*>;
	private:
	bool set(options&, const char*);
	std::string urlEncode(const std::string&);
	std::string getToken(const std::string&, const std::string&) const;
	time_t getTime(std::string&&, const std::string&) const;
	std::string getJson(std::string&&) const;
	public:
	bool getAccess();
	bool getSecret();
	Context();
	~Context();
	void parse(size_t, char**);
	friend std::ostream& operator<<(std::ostream& oss, const Context& context);
};
