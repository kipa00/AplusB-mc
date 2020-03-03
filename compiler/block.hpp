#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "predicate.hpp"

const byte AIR = 0;
const byte BEDROCK = 16;
const byte STONE = 32;
const byte SANDSTONE = 48;
const byte REDSTONE_WIRE = 64;
const byte REDSTONE_TORCH = 80;
const byte REDSTONE_WALL_TORCH = 96;
const byte COMPARATOR = 112;
const byte REPEATER = 128;
const byte REDSTONE_LAMP = 160;
const byte LEVER = 176;
const byte CONCRETE = 192;

const byte BLOCK_ONLY = 240;

const byte EAST = 0;
const byte NORTH = 1;
const byte SOUTH = 2;
const byte WEST = 3;
const byte POWERED = 4;
const byte SUBTRACT = 8;

const int ATTR_FACING = 1;
const int ATTR_FACE = 2;
const int ATTR_POWER = 4;
const int ATTR_POWERED = 8;
const int ATTR_LIT = 16;
const int ATTR_DELAY = 32;
const int ATTR_MODE = 64;
const int ATTR_LOCKED = 128;
const int ATTR_EAST = 256;
const int ATTR_WEST = 512;
const int ATTR_SOUTH = 1024;
const int ATTR_NORTH = 2048;

string block_to_string(byte block);
bool is_repeater(byte block);
bool is_opaque(byte block);

#endif
