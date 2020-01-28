#ifndef PREDICATE_HPP
#define PREDICATE_HPP

#include <string>
#include <vector>
using std::string;
using std::vector;

typedef unsigned char byte;
typedef unsigned int u32;

string bytes_to_string(byte *data, int len);

// errors
const int REDSTONE_POWER_ERROR = -1;
const int REDSTONE_NONDUST_POWERED_ERROR = -2;
const int REDSTONE_REPEATER_DELAY_ERROR = -3;
const int REDSTONE_COMPARATOR_MODE_ERROR = -4;

#endif