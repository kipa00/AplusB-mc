#include "analyze.hpp"

inline int abs(int x) {
	return x < 0 ? -x : x;
}

int pack(int x, int y, int z) {
	const int cx = x >> 4, cy = y >> 4, cz = z >> 4;
	const int ix = x & 15, iy = y & 15, iz = z & 15;
	return (((cz << 9) | (cx << 4) | cy) << 12) | ((iz << 8) | (ix << 4) | iy);
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

bool redstone_into(const world &w, int x, int y, int z, int ox, int oy, int oz) {
	if (abs(x - ox) + abs(y - oy) + abs(z - oz) != 1) return false;
	if (x == ox && y - 1 == oy && z == oz) return true;
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
int analyze(const world &w, int x, int y, int z, vector<int> &result) {
	byte this_block = w.getXYZ(x, y, z);
	if ((this_block & BLOCK_ONLY) == REDSTONE_WIRE) {
		vector<int> side;
		for (int i=0; i<wide_len; ++i) {
			const int nx = x + wide_dx[i], ny = y + wide_dy[i], nz = z + wide_dz[i];
			byte fetched = w.getXYZ(nx, ny, nz);
			if ((fetched & BLOCK_ONLY) == REDSTONE_WIRE) {
				if (wide_dy[i] == 0 && abs(wide_dx[i]) + abs(wide_dz[i]) <= 1) {
					side.push_back(pack(nx, ny, nz));
				} else if (wide_dy[i] == 1 && !is_opaque(w.getXYZ(x, y + 1, z))) {
					side.push_back(pack(nx, ny, nz));
				} else if (wide_dy[i] == -1 && !is_opaque(w.getXYZ(nx, ny + 1, nz))) {
					side.push_back(pack(nx, ny, nz));
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
		result.push_back(-1);
		for (const int &x : side) {
			result.push_back(x);
		}
		return 1;
	} else if ((this_block & BLOCK_ONLY) == REDSTONE_TORCH || (this_block & BLOCK_ONLY) == REDSTONE_WALL_TORCH) {
		int bx = x, by = y, bz = z;
		if ((this_block & BLOCK_ONLY) == REDSTONE_TORCH) --by;
		else switch (this_block & 3) {
			case EAST: --bx; break;
			case WEST: ++bx; break;
			case SOUTH: --bz; break;
			case NORTH: ++bz; break;
		}
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
		return 2;
	} else if (is_repeater(this_block)) {
		test_facing(w, x, y, z, [&] (const world &w, int bx, int by, int bz) {
			byte facing = w.getXYZ(bx, by, bz);
			if (is_opaque(facing)) {
				for (int i=0; i<narrow_len; ++i) {
					const int nx = bx + narrow_dx[i], ny = by + narrow_dy[i], nz = bz + narrow_dz[i];
					byte fetched = w.getXYZ(nx, ny, nz);
					if ((fetched & BLOCK_ONLY) == REDSTONE_WIRE) {
						if (redstone_into(w, nx, ny, nz, bx, by, bz)) {
							result.push_back(pack(nx, ny, nz));
						}
					} else if ((fetched & BLOCK_ONLY) == REDSTONE_TORCH || (fetched & BLOCK_ONLY) == REDSTONE_WALL_TORCH) {
						if (ny - by == -1) {
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
			} else switch (facing & BLOCK_ONLY) {
				case REDSTONE_WIRE:
				case REDSTONE_TORCH:
				case REDSTONE_WALL_TORCH:
				case LEVER:
					result.push_back(pack(bx, by, bz));
					break;
				default:
					if (is_repeater(facing) || (facing & BLOCK_ONLY) == COMPARATOR) {
						if ((facing & 3) == (this_block & 3)) {
							result.push_back(pack(bx, by, bz));
						}
					}
					break;
			}
			result.push_back(-1);
			int dsx = -1, dex = 1, dsz = -1, dez = 1;
			if (z == bz) {
				dsx = dex = 0;
			} else {
				dsz = dez = 0;
			}
			for (int dx = dsx; dx <= dex; dx += 2) {
				for (int dz = dsz; dz <= dez; dz += 2) {
					if (is_repeater(w.getXYZ(x + dx, y, z + dz)) && test_opposite_facing(w, x + dx, y, z + dz, [=] (const world &w, int bx, int by, int bz) {
						return bx == x && by == y && bz == z;
					})) {
						result.push_back(pack(x + dx, y, z + dz));
					}
				}
			}
			return true;
		});
		return 3 + ((this_block >> 3) & 3);
	} else if ((this_block & BLOCK_ONLY) == COMPARATOR) {
		test_facing(w, x, y, z, [&] (const world &w, int bx, int by, int bz) {
			byte facing = w.getXYZ(bx, by, bz);
			if (is_opaque(facing)) {
				for (int i=0; i<narrow_len; ++i) {
					const int nx = bx + narrow_dx[i], ny = by + narrow_dy[i], nz = bz + narrow_dz[i];
					byte fetched = w.getXYZ(nx, ny, nz);
					if ((fetched & BLOCK_ONLY) == REDSTONE_WIRE) {
						if (redstone_into(w, nx, ny, nz, bx, by, bz)) {
							result.push_back(pack(nx, ny, nz));
						}
					} else if ((fetched & BLOCK_ONLY) == REDSTONE_TORCH || (fetched & BLOCK_ONLY) == REDSTONE_WALL_TORCH) {
						if (ny - by == -1) {
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
			} else switch (facing & BLOCK_ONLY) {
				case REDSTONE_WIRE:
				case REDSTONE_TORCH:
				case REDSTONE_WALL_TORCH:
				case LEVER:
					result.push_back(pack(bx, by, bz));
					break;
				default:
					if (is_repeater(facing) || (facing & BLOCK_ONLY) == COMPARATOR) {
						if ((facing & 3) == (this_block & 3)) {
							result.push_back(pack(bx, by, bz));
						}
					}
					break;
			}
			result.push_back(-1);
			int dsx = -1, dex = 1, dsz = -1, dez = 1;
			if (z == bz) {
				dsx = dex = 0;
			} else {
				dsz = dez = 0;
			}
			for (int dx = dsx; dx <= dex; dx += 2) {
				for (int dz = dsz; dz <= dez; dz += 2) {
					byte facing = w.getXYZ(x + dx, y, z + dz);
					if ((facing & BLOCK_ONLY) == REDSTONE_WIRE) {
						result.push_back(pack(x + dx, y, z + dz));
					} else if (is_repeater(facing) || (facing & BLOCK_ONLY) == COMPARATOR) {
						if (test_opposite_facing(w, x + dx, y, z + dz, [=] (const world &w, int bx, int by, int bz) {
							return bx == x && by == y && bz == z;
						})) {
							result.push_back(pack(x + dx, y, z + dz));
						}
					}
				}
			}
			return true;
		});
		return 7 + (this_block & SUBTRACT ? 1 : 0);
	} else if ((this_block & BLOCK_ONLY) == LEVER) {
		return 9;
	} else if ((this_block & BLOCK_ONLY) == REDSTONE_LAMP) {
		for (int i=0; i<narrow_len; ++i) {
			const int nx = x + narrow_dx[i], ny = y + narrow_dy[i], nz = z + narrow_dz[i];
			byte fetched = w.getXYZ(nx, ny, nz);
			if (is_opaque(fetched)) {
				for (int j=0; j<narrow_len; ++j) {
					const int bx = nx + narrow_dx[j], by = ny + narrow_dy[j], bz = nz + narrow_dz[j];
					byte facing = w.getXYZ(bx, by, bz);
					if ((facing & BLOCK_ONLY) == REDSTONE_WIRE) {
						if (redstone_into(w, bx, by, bz, nx, ny, nz)) {
							result.push_back(pack(bx, by, bz));
						}
					} else if ((facing & BLOCK_ONLY) == REDSTONE_TORCH || (facing & BLOCK_ONLY) == REDSTONE_WALL_TORCH) {
						if (by - ny == -1) {
							result.push_back(pack(bx, by, bz));
						}
					} else if (is_repeater(facing) || (facing & BLOCK_ONLY) == COMPARATOR) {
						if (test_opposite_facing(w, bx, by, bz, [=] (const world &w, int xx, int yy, int zz) {
							return nx == xx && ny == yy && nz == zz;
						})) {
							result.push_back(pack(bx, by, bz));
						}
					} else if ((facing & BLOCK_ONLY) == LEVER) {
						if (test_opposite_facing(w, bx, by, bz, [=] (const world &w, int xx, int yy, int zz) {
							return nx == xx && ny == yy && nz == zz;
						})) {
							result.push_back(pack(bx, by, bz));
						}
					}
				}
			} else if ((fetched & BLOCK_ONLY) == REDSTONE_WIRE) {
				if (redstone_into(w, nx, ny, nz, x, y, z)) {
					result.push_back(pack(nx, ny, nz));
				}
			} else if ((fetched & BLOCK_ONLY) == REDSTONE_TORCH || (fetched & BLOCK_ONLY) == REDSTONE_WALL_TORCH) {
				if (ny - y <= 0 || (fetched & BLOCK_ONLY) == REDSTONE_WALL_TORCH) {
					result.push_back(pack(nx, ny, nz));
				}
			} else if (is_repeater(fetched) || (fetched & BLOCK_ONLY) == COMPARATOR) {
				if (test_opposite_facing(w, nx, ny, nz, [=] (const world &w, int xx, int yy, int zz) {
					return x == xx && y == yy && z == zz;
				})) {
					result.push_back(pack(nx, ny, nz));
				}
			} else if ((fetched & BLOCK_ONLY) == LEVER) {
				if (test_opposite_facing(w, nx, ny, nz, [=] (const world &w, int xx, int yy, int zz) {
					return x == xx && y == yy && z == zz;
				})) {
					result.push_back(pack(nx, ny, nz));
				}
			}
		}
		return 10;
	}
	return 0;
}
