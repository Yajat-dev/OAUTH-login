#include <iostream>
#include <string>
#include <unistd.h>

#include "file.hpp"
#include "context.hpp"

using namespace std;

bool Context::set(options& option, const char* arg)		// return value means data consumed
{
	if (get_if<monostate>(&option)) return false;
	if (!*arg) return true;
	try {
		if (auto param = get_if<int*>(&option)) **param = stoll(arg, nullptr, 0);
		else if (auto param = get_if<string*>(&option)) **param = arg;
	}
	catch (exception e) {}
	option = monostate{};
	return true;
}

void Context::parse(size_t n, char** argv)
{
	options option = monostate{};
	bool nomore = false;			// no more options

	for (int i = 1; i < n; i++) {
		char* arg = argv[i];
		if (!nomore && *arg == '-') {
			option = monostate{};
			while (*++arg){
				// no parameter options
				if (*arg == 'h') help = true;
				else if (*arg == '-') nomore = true;
				else if (*arg == 'c') confirm = true;
				else if (*arg == 'v') { if (verbose) debug = true; else verbose = true; }
				// options with parameter
				else if (*arg == 'r') option = &retry;
				else if (*arg == 'l') { log = home / ".mutt" / "oauth.login.log"; option = &log; }
				// options with parameter
				if (set(option, arg + 1)) break;
			}
		}
		else if (option.index()) set(option, arg);
		else hint = arg;
	}

	if (help) cerr << argv[0] << R"EOF( [Options] email

Parameter:
email	gmail account

Options:
	no-parameter option may be combined following one leading -
	space after option letter may be omitted, parameters are consumed until next space
-h	display this help message and quit, helpfull to see other argument parsed
-v	be verbose, if repeated be more verbose with debug info
-l file	log to file, default ~/.mutt/oauth.login.log
-r N	number of retries
-c	stop to confirm some actions

Example:
)EOF" << argv[0] << " user123@gmail.com\n" << endl;
	cerr << "Parsed arguments:" << endl
		<< *this << endl;

	if (help) exit(EXIT_SUCCESS);
}

ostream& operator<<(ostream& oss, const Context& context) {
	oss << "e-mail:" << context.hint;
	if (!context.log.empty()) oss << ", log:" << context.log;
	if (context.verbose) {
		if (context.debug) oss << ", debug";
		else oss << ", verbose";
	}
	if (context.confirm) oss << ", confirm";
	oss << ", PID:" << getpid();
	if (context.state > Context::State::none) {
		oss << ", action:";
		if (context.state > Context::State::refresh) oss << "access";
		else if (context.state > Context::State::token) oss << "refresh";
		else oss << "token";
	}
	return oss << endl;
}

bool Context::stop()
{
	return !--retry;
}

Context::Context(){
	verbose = debug = confirm = false;
	state = State::none;
	retry = 0;
	home = getenv("HOME");
}

Context::~Context(){
	if (!log.empty()) {
		auto now = time(nullptr);
		ofstream file(log.c_str(), ios::app);
		file << put_time(localtime(&now), "%Y.%m.%d %H:%M:%S") << ' ' << *this;
	}

}
