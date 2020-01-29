#ifndef BITSTREAM_HPP
#define BITSTREAM_HPP

#include "predicate.hpp"

class bitreader {
private:
	const byte *data;
	int pos;
public:
	bitreader() {
		init(nullptr, false);
	}
	bitreader(const byte *buf, bool copy = false) {
		init(buf, copy);
	}
	void init(const byte *buf, bool copy = false);
	int read(int bit);
	bool init_needed();
};

#endif
