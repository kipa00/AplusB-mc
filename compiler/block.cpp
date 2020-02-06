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
		case REPEATER | 1: return "repeater";
		case REDSTONE_LAMP: return "redstone_lamp";
		case LEVER: return "lever";
		case LIME_CONCRETE: return "lime_concrete";
		case LIGHT_BLUE_CONCRETE: return "light_blue_concrete";
	}
	return "unknown";
}

inline bool is_repeater(byte block) {
	return (block & 224) == REPEATER;
}
