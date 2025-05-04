#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <arpa/inet.h>

#include "code.hpp"

using namespace std;

Code::Code()
{

}

char get(char c)
{
	if (c == 62) return '-';
	if (c == 63) return '_';
	constexpr size_t v = 'Z' - 'A' + 1;
	if (c < v) return 'A' + c;
	c -= v;
	if (c < v) return 'a' + c;
	c -= v;
	return '0' + c;
}
void Code::encode()
{
	unsigned int x = 0, y, i;
	base.clear();
	cerr << "encoding..." << endl;
	int b = 0;
	for (i = 0; i < CODE; i++) {
		x = (x << 8) + data[i];
		b += 8;
		while (b > 6) {
			b -= 6;
			y = (x >> b) & 0x3F;
			base.push_back(get(y));
			cerr << dec << i << '.' << hex << base.back() << '/' << x << '/' << b << '/' << y << endl;
		}
	}
	if (b > 0) {
		y = x & 0x3F;
		base.push_back(get(y));
		cerr << dec << i << '.' << hex << base.back() << '/' << x << '/' << b << '/' << y << endl;
	}
	ostringstream command;
	cerr << base << endl;
	command << "echo " << base << "|basenc --base64url -d|hd";
	system(command.str().c_str());
}

void Code::decode()
{
}

void Code::create()
{
	srand(time(NULL));
	size_t length = CODE/sizeof(unsigned int);
	auto data = reinterpret_cast<unsigned int*>(this->data);
	while (length--) 
		data[length] = rand();
}

ostream& operator<<(ostream& os, const Code& c)
{
	auto data = reinterpret_cast<const unsigned int*>(c.data);
	size_t length = CODE/sizeof(unsigned int);
	os << "Code: ";
	for (int i = 0; i < length; i++) os << hex << uppercase << setw(8) << std::setfill('0') << htonl(data[i]);
	os << endl << "Base: " << c.base << endl;
	return os;
}
