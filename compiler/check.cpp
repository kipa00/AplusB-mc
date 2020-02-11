#include "check.hpp"

bool is_redstone_component(const world &w, int x, int y, int z) {
	byte fetched = w.getXYZ(x, y, z);
	if ((fetched & BLOCK_ONLY) == REDSTONE_WALL_TORCH || (fetched & BLOCK_ONLY) == LEVER) {
		switch (fetched & 3) {
			case EAST: --x; break;
			case WEST: ++x; break;
			case SOUTH: --z; break;
			case NORTH: ++z; break;
		}
		if (!is_opaque(w.getXYZ(x, y, z))) {
			throw NO_SUPPORTING_BLOCK_ERROR;
		}
		return true;
	} else if ((fetched & BLOCK_ONLY) == REDSTONE_WIRE || (fetched & BLOCK_ONLY) == REDSTONE_TORCH || is_repeater(fetched) || (fetched & BLOCK_ONLY) == COMPARATOR) {
		if (!is_opaque(w.getXYZ(x, y-1, z))) {
			throw NO_SUPPORTING_BLOCK_ERROR;
		}
		return true;
	}
	return (fetched & BLOCK_ONLY) == REDSTONE_LAMP;
}
