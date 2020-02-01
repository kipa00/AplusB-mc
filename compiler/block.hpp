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
const byte LIME_CONCRETE = 192;
const byte LIGHT_BLUE_CONCRETE = 208;

const byte BLOCK_ONLY = 240;

const byte EAST = 0;
const byte NORTH = 1;
const byte SOUTH = 2;
const byte WEST = 3;
const byte POWERED = 4;
const byte SUBTRACT = 8;

const int NOT_SUPPORTED_LEVER = 0;

string block_to_string(byte block);
bool is_repeater(byte block);

#endif
