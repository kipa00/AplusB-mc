#include "bytestream.hpp"

bytereader::bytereader(const char *s) {
	fp = nullptr;
	buf = new byte[1048576];
	open(s);
}

bytereader::~bytereader() {
	close();
	delete []buf;
}

void bytereader::open(const char *s) {
	close();
	fp = fopen(s, "rb");
	pos = len = 0;
}

void bytereader::close() {
	if (fp) {
		fclose(fp);
		fp = nullptr;
		pos = len = 0;
	}
}

byte bytereader::read_byte() {
	if (pos == len) {
		if ((len = fread(buf, 1, 1048576, fp)) <= 0) {
			throw EOF_ERROR;
		}
		pos = 0;
	}
	return buf[pos++];
}

int bytereader::read_int() {
	u32 x;
	int i;
	for (i=0; i<4; ++i) {
		(x <<= 8) |= read_byte();
	}
	return (int)x;
}
