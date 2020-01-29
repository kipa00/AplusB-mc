#include "bitstream.hpp"

int bitreader::read(int bit) {
	int remaining = 8 - (this->pos & 7);
	if (remaining >= bit) {
		int last_pos = this->pos;
		this->pos += bit;
		return (this->data[last_pos >> 3] & ((1 << remaining) - 1)) >> (remaining - bit);
	} else {
		int val = this->data[this->pos >> 3] & ((1 << remaining) - 1);
		bit -= remaining;
		this->pos += remaining;
		while (bit >= 8) {
			(val <<= 8) |= this->data[this->pos >> 3];
			this->pos += 8;
		}
		if (bit > 0) {
			(val <<= bit) |= (this->data[this->pos >> 3] >> (8 - bit));
			this->pos += bit;
		}
		return val;
	}
}

void bitreader::init(const byte *data, bool copy) {
	// TODO: implement copy if needed
	this->data = data;
	this->pos = 0;
}

bool bitreader::init_needed() {
	return !this->data;
}
