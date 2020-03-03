#ifndef PREDICATE_HPP
#define PREDICATE_HPP

#include <string>
#include <vector>
using std::string;
using std::vector;

typedef unsigned char byte;
typedef unsigned int u32;
typedef unsigned long long u64;

string bytes_to_string(const byte *data, int len);

// errors
const int UNKNOWN_BLOCK_ERROR = -1;
const int ATTR_VALUE_MISMATCH_ERROR = -2;
const int DUPLICATE_Y_SECTION_ERROR = -3;

const int UNSUPPORTED_COMPRESSION_ERROR = -4;
const int TOO_BIG_SECTION_ERROR = -5;

const int NO_SUPPORTING_BLOCK_ERROR = -6;

const int UNKNOWN_ATTR_ERROR = -7;
const int ATTR_MISMATCH_ERROR = -8;
const int NOT_SUPPORTED_LEVER = -9;

#endif
