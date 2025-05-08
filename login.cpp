#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>

#include "context.hpp"

using namespace std;
Context context;

void getSecrets() {
	
}

int main(int n, char** argv) {
	context.parse(n, argv);
	auto now = time(nullptr);
	ofstream log(".oauth.login.log", ios::app);
	log << put_time(localtime(&now), "%Y.%m.%d %H:%M:%S") << ' ' << context.hint << endl;
	while (context.getAccess()
		&& context.getSecret());
    return 0;
}
