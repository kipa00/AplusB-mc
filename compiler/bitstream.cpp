#include "bitstream.hpp"

int bitreader::read(int bit) {
	int x = 0, u = 0;
	if (this->remaining < bit) {
		x = this->temp;
		bit -= this->remaining;
		u = this->remaining;
		this->remaining = 64;
		this->temp = 0;
		for (int i=0; i<8; ++i) {
			(this->temp <<= 8) |= this->data[this->pos++];
		}
	}
	x |= (this->temp & ((1ULL << bit) - 1)) << u;
	this->temp >>= bit;
	this->remaining -= bit;
	return x;
}

void bitreader::init(const byte *data, bool copy) {
	// TODO: implement copy if needed
	this->data = data;
	this->pos = 0;
	this->remaining = 0;
}

bool bitreader::init_needed() {
	return !this->data;
}
