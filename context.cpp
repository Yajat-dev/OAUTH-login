#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <string>
#include <sstream>

#include "helper.hpp"
#include "context.hpp"

bool Context::verbose = false;
bool Context::debug = false;
bool Context::confirm = false;

using namespace std;

bool Context::set(options& option, const char* arg)		// return value means data consumed
{
	if (get_if<monostate>(&option)) return false;
	if (!*arg) return true;
	try {
		if (auto param = get_if<string*>(&option)) **param = arg;
	}
	catch (exception e) {}
	option = monostate{};
	return true;
}

void Context::parse(size_t n, char** argv)
{
	options option = monostate{};

	for (int i = 1; i < n; i++) {
		char* arg = argv[i];
		if (*arg == '-') {
			option = monostate{};
			while (*++arg){
				// no parameter options
				if (*arg == 'h') help = true;
				else if (*arg == 'c') confirm = true;
				else if (*arg == 'v') { if (verbose) debug = true; else verbose = true; }
				// options with parameter
				if (set(option, arg + 1)) break;
			}
		}
		else if (option.index()) set(option, arg);
		else hint = arg;
	}

	if (help) cerr << argv[0] << R"EOF( [Options] <login hint>

Parameter:
file	filename to keep secrets

Options:
	no-parameter option may be combined following one leading -
	space after option letter may be omitted, parameters are consumed until next space
-h	display this help message and quit, helpfull to see other argument parsed
-v	be verbose, if repeated be more verbose with debug info
-c	stop to confirm some actions

Example:

)EOF";
	cerr << "Parsed arguments:\n" << *this;

	if (help) exit(EXIT_SUCCESS);
}

ostream& operator<<(ostream& oss, const Context& context) {
	oss << "mail hint: " << context.hint;
	if (context.verbose) {
		if (context.debug) oss << ", debug";
		else oss << ", verbose";
	}
	if (context.confirm) oss << ", confirm";
	return oss << endl;
}

Context::Context(){
	verbose = debug = confirm = false;
}

bool Context::connect()
{
	int client = socket(AF_INET, SOCK_STREAM, 0);
	int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        cerr << "Could not create socket: " << strerror(errno) << std::endl;
        return 1;
    }

    // Tworzymy strukturę sockaddr_in dla IPv4
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr)); // Zerujemy strukturę

    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(0); // Dowolny port (0 - system przypisuje port)

    // Łączymy gniazdo z adresem i portem
    if (bind(server, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        cerr << "Bin error: " << strerror(errno) << std::endl;
        close(server);
        return 1;
    }

    // Uzyskujemy przydzielony port
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    if (getsockname(server, (struct sockaddr*)&addr, &addr_len) == -1) {
        std::cerr << "Błąd przy pobieraniu portu: " << strerror(errno) << std::endl;
        close(server);
        return 1;
    }

	port = ntohs(addr.sin_port);
    cerr << "Listening on port: " << port << endl;
	ostringstream command;
	command << "xdg-open 'https://accounts.google.com/o/oauth2/auth/oauthchooseaccount?"
		<< "response_type=code" << '&'
		<< "redirect_uri=http://localhost:" << port << '&'
		<< "scope=https://www.googleapis.com/auth/gmail.readonly" << '&'
		<< "client_id=1066306325374-itr5ih1ivquo8hmi841ts7mumv2vn2k4.apps.googleusercontent.com" << '&'
		<< "code_challange=Agwo322ha" << '&'
		<< "code_challenge_method=S256" << '&'
		<< "login_hint=" << hint << '&' << '\''
		<< endl;
	cerr << command.str();
	system(command.str().c_str());
	listen(server, 1);
	int redir = accept(server, 0, 0);
	char request[1024];
	size_t n = recv(redir, request, sizeof(request), 0);
	request[n] = 0;
	istringstream input(request);
	getline(input, code, '=');
	cerr << "Authentication code/";
	getline(input, code, '&');
	n = 47 + code.size();
	cerr << n << ':' << code << endl;
	ostringstream output;
	output << "HTTP/1.1 200 OK" << endl
		<< "Content-Type: text/plain; charset=UTF-8" << endl
		<< "Content-Length: " << n << endl << endl
		<< "Authentication code: " << code <<  endl
		<< "You may close the window" << endl;
	send(redir, output.str().c_str(), output.str().size(), 0);

	return true;
}

Context::~Context() {
    // Zamykanie gniazda
    close(server);
    close(client);
}

string Context::verifier(size_t length)
{
	return "Under work";
}
