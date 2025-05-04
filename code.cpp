#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <arpa/inet.h>

#include "code.hpp"

using namespace std;

Code::Code()
{

}

char get(unsigned short c)
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

int get(char c)
{
	constexpr size_t v = 'Z' - 'A' + 1;
	if (c == '-') return 62; 
	if (c < '0') return -1; 
	if (c <= '9') return c - '0' + v + v; 
	if (c < 'A') return -1; 
	if (c <= 'Z') return c - 'A'; 
	if (c == '_') return 63; 
	if (c < 'a') return -1; 
	if (c <= 'z') return c - 'a' + v; 
	return -1;
}

void Code::encode()
{
	unsigned int x = 0;
	unsigned short y;
	cerr << "encoding" << endl;
	base.clear();
	int b = 0;
	for (int i = 0; i < CODE; i++) {
		x = (x << 8) | data[i];
		b += 8;
		while (b > 6) {
			b -= 6;
			y = (x >> b) & 0x3F;
			base.push_back(get(y));
		}
	}
	if (b > 0) {
		y = x & 0x3F;
		base.push_back(get(y));
	}
}

void Code::decode()
{
	unsigned int x = 0;
	int y, b = 0;
	size_t i = 0;
	memset(data, 0, sizeof(data));
	cerr << "decoding" << endl;
	for(auto c: base) {
		y = get(c);
		x = (x << 6) | y;
		b += 6;
		while (b > 8) {
			b -= 8;
			data[i++] = (x >> b) & 0xFF;
		}
	}
	if (y < 0) return;
	if (b > 0) {
		y = x & 0xFF;
		data[i] = y;
	}
}

void Code::create()		// random values of 96 bits in total
{
	srand(time(NULL));
	size_t length = CODE/sizeof(unsigned int);
	auto data = reinterpret_cast<unsigned int*>(this->data);
	while (length--) 
		data[length] = rand();
}

ostream& operator<<(ostream& os, const Code& c)
{
	os << "Code:";
	for (int i = 0; i < CODE; i++) os << ' ' << hex << uppercase << setw(2) << std::setfill('0') << int(c.data[i]);
	os << endl << "Base: " << c.base << endl;
	return os;
}
