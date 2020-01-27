#include <cstdio>
#include "zlib.h"
#include "chunk.hpp"

const int BUFSIZE = 1048576;
byte data[BUFSIZE];

int main() {
	FILE *fp = fopen("inflated.bin", "rb");
	int len = fread(data, 1, BUFSIZE, fp);
	fclose(fp);
	printf("%d\n", len);

	chunk c;
	c.read(data);
	return 0;
}
