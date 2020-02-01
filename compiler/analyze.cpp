#include "analyze.hpp"

bool is_opaque(byte block) {
	switch (block & BLOCK_ONLY) {
		case BEDROCK:
		case STONE:
		case SANDSTONE:
		case REDSTONE_LAMP:
		case LIME_CONCRETE:
		case LIGHT_BLUE_CONCRETE:
			return true;
	}
	return false;
}

inline int abs(int x) {
	return x < 0 ? -x : x;
}

inline int pack(int x, int y, int z) {
	return (x << 17) | (y << 9) | z;
}

bool test_facing(const world &w, int x, int y, int z, std::function<bool(const world &, int, int, int)> f) {
	byte b = w.getXYZ(x, y, z);
	switch (b & 3) {
		case EAST:
			++x;
			break;
		case WEST:
			--x;
			break;
		case SOUTH:
			++z;
			break;
		case NORTH:
			--z;
			break;
	}
	return f(w, x, y, z);
}

bool test_opposite_facing(const world &w, int x, int y, int z, std::function<bool(const world &, int, int, int)> f) {
	byte b = w.getXYZ(x, y, z);
	switch (b & 3) {
		case EAST:
			--x;
			break;
		case WEST:
			++x;
			break;
		case SOUTH:
			--z;
			break;
		case NORTH:
			++z;
			break;
	}
	return f(w, x, y, z);
}

inline bool is_repeater(byte block) {
	return (block & 224) == REPEATER;
}

bool redstone_into(const world &w, int x, int y, int z, int ox, int oy, int oz) {
	if (abs(x - ox) + abs(y - oy) + abs(z - oz) != 1) return false;
	if (y != oy) return false;

	bool block_upper = is_opaque(w.getXYZ(x, y + 1, z));
#define f(dx,dz) (\
	(w.getXYZ(x + dx, y, z + dz) & BLOCK_ONLY) == REDSTONE_WIRE\
	|| ((w.getXYZ(x + dx, y + 1, z + dz) & BLOCK_ONLY) == REDSTONE_WIRE && !block_upper)\
	|| ((w.getXYZ(x + dx, y - 1, z + dz) & BLOCK_ONLY) == REDSTONE_WIRE && !is_opaque(w.getXYZ(x + dx, y, z + dz)))\
)
	bool east = f(1, 0);
	bool west = f(-1, 0);
	bool south = f(0, 1);
	bool north = f(0, -1);
#undef f
	int flag = east + west + south + north;
	if (!flag) {
		east = west = south = north = true;
	} else if (flag == 1) {
		if (east) west = true;
		else if (west) east = true;
		else if (south) north = true;
		else if (north) south = true;
	}
	if (x + 1 == ox) return east;
	else if (x - 1 == ox) return west;
	else if (z + 1 == oz) return south;
	else if (z - 1 == oz) return north;
	return false;
}

// world must be error-free
void analyze(const world &w, int x, int y, int z, vector<int> &result) {
	byte this_block = w.getXYZ(x, y, z);
	if ((this_block & BLOCK_ONLY) == REDSTONE_WIRE) {
		for (int i=0; i<wide_len; ++i) {
			const int nx = x + wide_dx[i], ny = y + wide_dy[i], nz = z + wide_dz[i];
			byte fetched = w.getXYZ(nx, ny, nz);
			if ((fetched & BLOCK_ONLY) == REDSTONE_WIRE) {
				if (wide_dy[i] == 0 && abs(wide_dx[i]) + abs(wide_dz[i]) <= 1) {
					result.push_back(pack(nx, ny, nz));
				} else if (wide_dy[i] == 1 && !is_opaque(w.getXYZ(x, y + 1, z))) {
					result.push_back(pack(nx, ny, nz));
				} else if (wide_dy[i] == -1 && !is_opaque(w.getXYZ(nx, ny + 1, nz))) {
					result.push_back(pack(nx, ny, nz));
				}
			} else if ((fetched & BLOCK_ONLY) == REDSTONE_TORCH || (fetched & BLOCK_ONLY) == REDSTONE_WALL_TORCH) {
				if (
					abs(nx - x) + abs(ny - y) + abs(nz - z) <= 1 // direct connection
					|| (is_opaque(w.getXYZ(nx, ny + 1, nz)) && abs(nx - x) + abs(ny + 1 - y) + abs(nz - z) <= 1) // indirect connection
				) {
					result.push_back(pack(nx, ny, nz));
				}
			} else if (is_repeater(fetched) || (fetched & BLOCK_ONLY) == COMPARATOR) {
				if (test_opposite_facing(w, nx, ny, nz, [=] (const world &w, int xx, int yy, int zz) {
					return
						(xx == x && yy == y && zz == z) // direct connection
						|| (is_opaque(w.getXYZ(xx, yy, zz)) && abs(xx - x) + abs(yy - y) + abs(zz - z) <= 1); // indirect connection
					})
				) {
					result.push_back(pack(nx, ny, nz));
				}
			} else if ((fetched & BLOCK_ONLY) == LEVER) {
				if (
					abs(nx - x) + abs(ny - y) + abs(nz - z) <= 1 // direct connection
					|| test_opposite_facing(w, nx, ny, nz, [=] (const world &w, int xx, int yy, int zz) {
						return is_opaque(w.getXYZ(xx, yy, zz)) && abs(xx - x) + abs(yy - y) + abs(zz - z) <= 1;
					})
				) {
					result.push_back(pack(nx, ny, nz));
				}
			}
		}
	} else if ((this_block & BLOCK_ONLY) == REDSTONE_TORCH || (this_block & BLOCK_ONLY) == REDSTONE_WALL_TORCH) {
		int bx = x, by = y, bz = z;
		if ((this_block & BLOCK_ONLY) == REDSTONE_TORCH) --by;
		else switch (this_block & 3) {
			case EAST: --bx; break;
			case WEST: ++bx; break;
			case SOUTH: --bz; break;
			case NORTH: ++bz; break;
		}
		//printf("TORCH AT (%d, %d, %d), BASE BLOCK AT (%d, %d, %d)\n", x, y, z, bx, by, bz);
		for (int i=0; i<wide_len; ++i) {
			const int nx = x + wide_dx[i], ny = y + wide_dy[i], nz = z + wide_dz[i];
			byte fetched = w.getXYZ(nx, ny, nz);
			if ((fetched & BLOCK_ONLY) == REDSTONE_WIRE) {
				if (redstone_into(w, nx, ny, nz, bx, by, bz)) {
					result.push_back(pack(nx, ny, nz));
				}
			} else if ((fetched & BLOCK_ONLY) == REDSTONE_TORCH || (fetched & BLOCK_ONLY) == REDSTONE_WALL_TORCH) {
				if (nx == bx && ny == by - 1 && nz == bz) {
					result.push_back(pack(nx, ny, nz));
				}
			} else if (is_repeater(fetched) || (fetched & BLOCK_ONLY) == COMPARATOR) {
				if (test_opposite_facing(w, nx, ny, nz, [=] (const world &w, int xx, int yy, int zz) {
					return bx == xx && by == yy && bz == zz;
				})) {
					result.push_back(pack(nx, ny, nz));
				}
			} else if ((fetched & BLOCK_ONLY) == LEVER) {
				if (test_opposite_facing(w, nx, ny, nz, [=] (const world &w, int xx, int yy, int zz) {
					return bx == xx && by == yy && bz == zz;
				})) {
					result.push_back(pack(nx, ny, nz));
				}
			}
		}
	}
}
