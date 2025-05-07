#include <iostream>

#include "context.hpp"

using namespace std;
Context context;

void getSecrets() {
	
}

int main(int n, char** argv) {
	context.parse(n, argv);
	if (context.getAccess())
		context.getToken();
    return 0;
}
