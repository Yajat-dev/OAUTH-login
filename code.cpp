#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>

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

string Code::encode(const unsigned char* data, size_t length) const
{
	unsigned int x = 0;
	unsigned short y;
	string res;
	int b = 0;
	for (int i = 0; i < length; i++) {
		x = (x << 8) | data[i];
		b += 8;
		while (b > 5) {
			b -= 6;
			y = (x >> b) & 0x3F;
			res.push_back(get(y));
		}
	}
	if (b > 0) {
		y = (x << (6 - b)) & 0x3F;
		res.push_back(get(y));
	}
	return res;
}

void Code::decode()
{
	unsigned int x = 0;
	int y, b = 0;
	size_t i = 0;
	memset(data, 0, sizeof(data));
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

void Code::draw()		// random values of 96 bits in total
{
	srand(time(NULL));
	size_t length = CODE/sizeof(unsigned int);
	auto data = reinterpret_cast<unsigned int*>(this->data);
	while (length--) 
		data[length] = rand();
}

ostream& operator<<(ostream& os, const Code& c)
{
	cerr << "Code: "; for (int i = 0; i < CODE; i++) os << hex << uppercase << setw(2) << setfill('0') << (0xFF & int(c.data[i])); os << endl;
	cerr << "Base: "; os << c.base << endl;
	cerr << "Sha: "; for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) os << hex << uppercase << setw(2) << setfill('0') << (0xFF & int(c.hash[i])); os << endl;
	return os;
}
