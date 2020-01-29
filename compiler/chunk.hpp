#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "predicate.hpp"
#include "block.hpp"
#include "bitstream.hpp"

class chunk {
private:
	int x, y, z, block;
	vector<int> palette;
	byte **world_data;
	bitreader br;

	void feed(int y);
	void feed(string name);
	void feed(string attr, string val);
	void feed(const byte *data, int len);
	void flush_section();
	void flush_block();
	bool read(const byte *data, int *pos, int tag_type = -1, int stage = 0);
	void init();
public:
	void read(const byte *data);
	byte getXYZ(int x, int y, int z);
	~chunk();
};

#endif
