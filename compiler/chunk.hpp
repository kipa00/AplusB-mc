#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "predicate.hpp"

class chunk {
private:
	int x, z;
	void feed(int y);
	void feed(string name);
	void feed(string attr, string val);
	void feed(byte *data, int len);
	void flush_section();
	void flush_block();
	bool read(byte *data, int *pos, int tag_type = -1, int stage = 0);
public:
	void read(byte *data);
};

#endif
