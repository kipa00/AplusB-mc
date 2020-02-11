#include "block.hpp"

string block_to_string(byte block) {
	switch (block & BLOCK_ONLY) {
		case AIR: return "air";
		case BEDROCK: return "bedrock";
		case STONE: return "stone";
		case SANDSTONE: return "sandstone";
		case REDSTONE_WIRE: return "redstone_wire";
		case REDSTONE_TORCH:
		case REDSTONE_WALL_TORCH: return "redstone_torch";
		case COMPARATOR: return "comparator";
		case REPEATER:
		case REPEATER | 16: return "repeater";
		case REDSTONE_LAMP: return "redstone_lamp";
		case LEVER: return "lever";
		case CONCRETE: return "concrete";
	}
	return "unknown";
}

bool is_repeater(byte block) {
	return (block & 224) == REPEATER;
}

bool is_opaque(byte block) {
	switch (block & BLOCK_ONLY) {
		case BEDROCK:
		case STONE:
		case SANDSTONE:
		case REDSTONE_LAMP:
		case CONCRETE:
			return true;
	}
	return false;
}
