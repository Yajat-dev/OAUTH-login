#include <iostream>
#include <openssl/sha.h>

#define CODE 48

class Code {
	private:
		unsigned char data[CODE];
		unsigned char hash[SHA256_DIGEST_LENGTH];  // 32 bajty = 256 bitów
		std::string base;
		std::string encode(const unsigned char*, size_t) const;
	public:
		Code();
		void create();
		std::string base64() { return base; }
		void encode() { base = encode(data, CODE); }
		void encrypt() { SHA256((const unsigned char*)base.c_str(), base.size(), hash); }
		std::string challenge() const { return encode(hash, SHA256_DIGEST_LENGTH); }
		void decode();
		friend std::ostream& operator<<(std::ostream&s, const Code&);
};

