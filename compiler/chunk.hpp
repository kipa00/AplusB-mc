#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "predicate.hpp"
#include "block.hpp"
#include "bitstream.hpp"
#include "color.hpp"

class chunk {
private:
	int x, y, z, block, attr;
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
	bool has_world_data(int cy) const;
	~chunk();
};

#endif
