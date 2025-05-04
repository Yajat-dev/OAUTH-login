#include <iostream>

#define CODE 12

class Code {
	private:
		unsigned char data[CODE];
		std::string base;
	public:
		Code();
		void create();
		void encode();
		void decode();
		friend std::ostream& operator<<(std::ostream&s, const Code&);
};

