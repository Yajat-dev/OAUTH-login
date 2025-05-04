#include <iostream>

#include "helper.hpp"
#include "code.hpp"
#include "context.hpp"

using namespace std;
Context context;

void getSecrets() {
	
}

int main(int n, char** argv) {

	context.parse(n, argv);
	// context.connect();
	context.verifier(16);
	Code verifier;
	verifier.create();
	cerr << verifier;
	verifier.encode();
	cerr << verifier;
	verifier.decode();
	cerr << verifier;
    return 0;
}
