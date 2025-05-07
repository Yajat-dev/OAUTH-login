#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <utime.h>

#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "context.hpp"

#define CLIENT_ID "1066306325374-itr5ih1ivquo8hmi841ts7mumv2vn2k4.apps.googleusercontent.com"
#define CLIENT_SECRET "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"

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
	cerr << "Parsed arguments:" << endl
		<< *this << endl;

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

bool Context::getAccess()
{
	ifstream secret(".token");
	if (secret) {
		string token;
		secret >> token;
		cout << token;
		return false;
	}

	int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) { cerr << "Could not create socket: " << strerror(errno) << std::endl; return 1; }

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
	crypto.create();
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
		<< url_encode(options.str())
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

void Context::getToken() {
	const char* host = "oauth2.googleapis.com";
	const char* port = "443";
	SSL_library_init();
    SSL_load_error_strings();

    cerr << endl << "About to get access token..." << endl;
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        cerr << "Can not create SSL context" << endl;
        return;
    }

    // 3. Rozwiązanie hosta i połączenie TCP
    struct addrinfo *res;
    if (getaddrinfo(host, port, NULL, &res)) {
		cerr << "Can not get address info for: " << host << endl
			<< strerror(errno) << endl;
        return;
    }

    int client = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (::connect(client, res->ai_addr, res->ai_addrlen) != 0) {
        cerr << "Can not connect client socket to: " << host << endl;
        return;
    }

    // 4. Tworzymy obiekt SSL i łączymy z socketem
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client);

    if (SSL_connect(ssl) < 1) {
        cerr << "Can not connect client SSL socket: ";
        ERR_print_errors_fp(stderr);
        return;
    }

    cerr << "TLS/SSL handshake OK with " << host << "\n";
	ostringstream output;
	ostringstream content;
	content << "code=" << code << '&'
		// << "scope=https://mail.google.com/" << '&'
		<< "client_id=" CLIENT_ID << '&'
		<< "client_secret=" CLIENT_SECRET << '&'
		<< "code_verifier=" << crypto.base64() << '&'
		<< "redirect_uri=http://127.0.0.1:" << this->port << '&'
		<< "grant_type=authorization_code";
	string append = content.str();
	output << "POST /token HTTP/1.1" << endl
		<< "Host: oauth2.googleapis.com" << endl
		<< "Content-Type: application/x-www-form-urlencoded" << endl
		<< "Content-Length: " << append.size() << endl
		<< endl
		<< append;
	cerr << endl << "Sending: " << output.str() << endl;
	SSL_write(ssl, output.str().c_str(), output.str().size());
	cerr << endl << "Getting access token..." << endl;
	string response;
	response.resize(2048);
	auto n = SSL_read(ssl, (void*)response.data(), response.capacity()); 
	response.resize(n);
	cerr << endl << response << endl;
	auto token = response.find("access_token");
	if (token == -1) { cerr << "Token not found:" << response << endl; return; }
	istringstream keys(response.substr(token));
	string key;
	getline(keys, key, ' ');
	getline(keys, key, '"');
	getline(keys, key, '"');
	ofstream secret(".token");
	secret << key;
	cout << key;
	utimbuf times;
	times = {time(0), time(0)};
	utime(".token", &times);
}

string Context::url_encode(const string& value) {
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
