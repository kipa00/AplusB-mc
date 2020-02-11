#ifndef BYTESTREAM_HPP
#define BYTESTREAM_HPP

#include "predicate.hpp"

class bytereader {
private:
	FILE *fp;
	byte *buf;
	int pos, len;
public:
	bytereader(const char *);
	~bytereader();
	void open(const char *);
	void close();
	byte read_byte();
	int read_int();
};

#endif
