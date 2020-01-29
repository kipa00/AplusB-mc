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
	int x, z;
	puts("z\\x 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
	for (z=0; z<16; ++z) {
		printf("%2X ", z);
		for (x=0; x<16; ++x) {
			printf("%02X ", c.getXYZ(x, 56, z));
		}
		puts("");
	}
	return 0;
}
