#include <cstdio>
#include "world.hpp"

const int BUFSIZE = 1048576;
byte data[BUFSIZE];

int main(int argc, char *argv[]) {
	FILE *fp = fopen(argv[1], "rb");
	world w;
	try {
		w.init(fp);
	} catch (int err) {
		printf("ERROR! %d\n", err);
		return -1;
	}
	fclose(fp);

	int x, z;
	puts("z\\x 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
	for (z=0; z<32; ++z) {
		printf("%2X ", z & 15);
		for (x=0; x<32; ++x) {
			printf("%02X ", w.getXYZ(x, 56, z));
		}
		puts("");
	}
	return 0;
}
