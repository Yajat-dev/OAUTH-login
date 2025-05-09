#include <iostream>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <utime.h>
#include <cstdlib>

#include <string>
#include <sstream>
#include <iomanip>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "file.hpp"
#include "context.hpp"

#define CLIENT_ID "1066306325374-itr5ih1ivquo8hmi841ts7mumv2vn2k4.apps.googleusercontent.com"
#define CLIENT_SECRET "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"
#define MUTT_CONFIG "/.mutt/accounts/"

using namespace std;

bool Context::set(options& option, const char* arg)		// return value means data consumed
{
	if (get_if<monostate>(&option)) return false;
	if (!*arg) return true;
	try { if (auto param = get_if<string*>(&option)) **param = arg; }
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
	cerr << "Parsed arguments:" << endl
		<< *this << endl;

	if (help) exit(EXIT_SUCCESS);
}

ostream& operator<<(ostream& oss, const Context& context) {
	oss << "e-mail:" << context.hint;
	if (context.verbose) {
		if (context.debug) oss << ", debug";
		else oss << ", verbose";
	}
	if (context.confirm) oss << ", confirm";
	oss << ", PID: " << getpid();
	return oss << endl;
}

Context::Context(){
	verbose = debug = confirm = false;
	home = getenv("HOME");
}

bool Context::getAccess()
{
	File access(home + MUTT_CONFIG + hint + "/access_token");
	if (access) {
		struct stat info;
		stat(access.getName().c_str(), &info);
		if (verbose) cerr << "Found access token file: " << access.getName();
		if (info.st_mtime > time(0)) {
			access >> secret;
			if (!secret.empty()) {
				if (verbose) cerr << endl;
				cout << secret;
				return false;
			}
		}
		if (verbose) cerr << "... but token has expired or been revoked" << endl;
	}

	File reaccess(home + MUTT_CONFIG + hint + "/refresh_token");
	if (reaccess) {
		if (verbose) cerr << "Found refresh token file: " << reaccess.getName();
		reaccess >> refresh;
		if (!refresh.empty()) return true;
		if (verbose) cerr << "... but it is empty" << endl;
	}

	int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
		cerr << "Could not create socket: " << strerror(errno) << std::endl;
		return false;
	}

    // Tworzymy strukturę sockaddr_in dla IPv4
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(0);

    if (bind(server, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        cerr << "Bind error: " << strerror(errno) << std::endl;
        close(server);
        return false;
    }

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    if (getsockname(server, (struct sockaddr*)&addr, &addr_len) == -1) {
        std::cerr << "Error getting port number: " << strerror(errno) << std::endl;
        close(server);
        return false;
    }

	port = ntohs(addr.sin_port);
    cerr << "Listening on port: " << port << endl;
	crypto.draw();
	crypto.encode();
	crypto.encrypt();
	cerr << crypto;
	ostringstream command, options;
	options << "response_type=code" << '&'
		<< "redirect_uri=http://127.0.0.1:" << port << '&'
		<< "scope=https://mail.google.com/" << '&'
		<< "client_id=" CLIENT_ID << '&'
		<< "code_challenge=" << crypto.challenge() << '&'
		<< "code_challenge_method=S256" << '&'
		<< "include_granted_scopes=true" << '&'
		// << "service=lso" << '&'
		// << "o2v=2" << '&'
		// << "flowName=GeneraloAuthFlow" << '&'
		// << "access_type=offline" << '&' 
		// << "prompt=consent" << '&'
		<< "login_hint=" << hint;
	command << "xdg-open 'https://accounts.google.com/o/oauth2/v2/auth?"
		<< urlEncode(options.str())
		<< "' >/dev/null";
	cerr << endl << command.str() << endl;
	system(command.str().c_str());
	listen(server, 1);
	int redir = accept(server, 0, 0);
	char request[1024];
	size_t n = recv(redir, request, sizeof(request), 0);
	request[n] = 0;
	cerr << endl << "Redirected data: " << endl << request;
	istringstream input(request);
	getline(input, code, '=');
	getline(input, code, '&');
	cerr << "Authentication code:" << code << endl;
	ostringstream output, content;
	content // << "Authentication code: " << code <<  endl
		<< "You may close the window" << endl;
	output << "HTTP/1.1 200 OK" << endl
		<< "Content-Type: text/plain; charset=UTF-8" << endl
		<< "Content-Length: " << content.str().size() << endl
		<< endl
		<< content.str() << endl;
	send(redir, output.str().c_str(), output.str().size(), 0);
	return true;
}

Context::~Context() {
    // Zamykanie gniazda
    close(server);
    close(client);
}

bool Context::getSecret()
{
	const char* host = "oauth2.googleapis.com";
	const char* port = "443";
	SSL_library_init();
    SSL_load_error_strings();

    if (debug) cerr << endl << "About to get access token..." << endl;
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        cerr << "Can not create SSL context" << endl;
        return false;
    }

    // 3. Rozwiązanie hosta i połączenie TCP
    struct addrinfo *res;
    if (getaddrinfo(host, port, NULL, &res)) {
		cerr << "Can not get address info for: " << host << endl
			<< strerror(errno) << endl;
        return false;
    }

    int client = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (::connect(client, res->ai_addr, res->ai_addrlen) != 0) {
        cerr << "Can not connect client socket to: " << host << endl;
        return false;
    }

    // 4. Tworzymy obiekt SSL i łączymy z socketem
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client);

    if (SSL_connect(ssl) < 1) {
        cerr << "Can not connect client SSL socket: ";
        ERR_print_errors_fp(stderr);
        return false;
    }

    if (debug) cerr << "TLS/SSL handshake OK with " << host << "\n";
	ostringstream output;
	ostringstream content;
	content 
		<< "client_id=" CLIENT_ID << '&'
		<< "client_secret=" CLIENT_SECRET << '&';
	if (refresh.empty()) {
		content << "code=" << code << '&'
		<< "code_verifier=" << crypto.base64() << '&'
		<< "redirect_uri=http://127.0.0.1:" << this->port << '&'
		<< "grant_type=authorization_code";
	} else {
		content << "refresh_token=" << refresh << '&'
			<< "grant_type=refresh_token";
	}
	string append = content.str();
	output << "POST /token HTTP/1.1" << endl
		<< "Host: oauth2.googleapis.com" << endl
		<< "Content-Type: application/x-www-form-urlencoded" << endl
		<< "Content-Length: " << append.size() << endl
		<< endl
		<< append;
	if (debug) cerr << endl << output.str() << endl;
	SSL_write(ssl, output.str().c_str(), output.str().size());
	cerr << endl << "Getting access token..." << endl;
	string response;
	response.resize(2048);
	auto n = SSL_read(ssl, (void*)response.data(), response.capacity()); 
	response = getJson(std::move(response));
	if (debug) cerr << response << endl << endl;

	secret = getToken("access_token", response);
	if (secret.empty() || refresh.empty()) refresh = getToken("refresh_token", response);
	if (secret.empty()) return true; // repeat whole sequence
	cout << secret;
	auto time = getTime("expires_in", response);

	utimbuf times = {0, time};
	string name = home + MUTT_CONFIG + hint + "/access_token";
	utime(name.c_str(), &times);
	return false;
}

string Context::getJson(string&& text) const
{
	string value;
	auto start = text.find('{');
	if (start == -1) return value;
	auto stop = text.find('}', start);
	if (stop == -1) return value;
	size_t n = 0;
	if (stop < start) return value;
	return text.substr(start, stop - start + 1);
}

string Context::getToken(const string& token, const string& text) const
{
	string value;
	auto start = text.find(token);
	if (start != -1) {
		istringstream keys(text.substr(start));
		getline(keys, value, ':');
		getline(keys, value, '"');
		getline(keys, value, '"');
	}
	File secret(home + MUTT_CONFIG + hint + '/' + token);
	secret << value;
	return value;
}

time_t Context::getTime(string&& token, const string& text) const
{
	unsigned long value = 0;
	string dummy;
	auto start = text.find(token);
	if (start == -1) return value;
	istringstream keys(text.substr(start));
	getline(keys, dummy, ':');
	keys >> value;
	return value + time(0);
}

string Context::urlEncode(const string& value) {
    std::ostringstream encoded;
    for (unsigned char c : value) {
        // Znaki alfanumeryczne oraz '-', '_', '.', '~' nie są kodowane
        if (isalnum(c) || c == '&' || c == '=' || c == '-' || c == '_' || c == '.' || c == '~') encoded << c;
        else encoded << '%' << uppercase << hex << setw(2) << setfill('0') << static_cast<int>(c);
    }
    return encoded.str();
}

/* link wysyłany z vivaldi do autoryzacji poczty gmail
 * https://accounts.google.com/o/oauth2/v2/auth/oauthchooseaccount?
 * client_id=17077306336-p93k4ki7ro01jq6804ahikkkroia6c1u.apps.googleusercontent.com&
 * redirect_uri=https%3A%2F%2Fmpognobbkildjkofajifpdfhcoklimli.chromiumapp.org%2F&
 * response_type=code&
 * code_challenge=3tRiF9cXHBKLRF2D8m537nJlOAGUmSbZpnpexBkodkg&
 * code_challenge_method=S256&
 * access_type=offline&
 * prompt=consent&
 * scope=https%3A%2F%2Fmail.google.com%2F%20
 * https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fcalendar%20
 * https%3A%2F%2Fwww.googleapis.com%2Fauth%2Ftasks%20openid%20
 * https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.email&
 * login_hint=grzmot969%40gmail.com&
 * include_granted_scopes=true&
 * service=lso&
 * o2v=2&
 * flowName=GeneralOAuthFlow
 */
