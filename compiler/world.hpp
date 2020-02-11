#ifndef WORLD_HPP
#define WORLD_HPP

#include "predicate.hpp"
#include "chunk.hpp"
#include "zlib.h"

class world {
private:
	chunk **world_data;

public:
	world();
	~world();
	void init(FILE *fp);
	byte getXYZ(int x, int y, int z) const;
	bool has_world_data(int cx, int cy, int cz) const;
};

#endif
