#include "predicate.hpp"

string bytes_to_string(const byte *data, int len) {
	char *str = new char[len + 1];
	memcpy(str, data, len);
	str[len] = 0;
	string res(str);
	delete []str;
	return res;
}
