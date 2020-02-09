#include <cstdio>
#include "world.hpp"
#include "analyze.hpp"

const int BUFSIZE = 1048576;
byte data[BUFSIZE];

int main(int argc, char *argv[]) {
	if (argc <= 1) {
		fprintf(stderr, "Usage: %s <region file name>\n", argv[0]);
		return -1;
	}
	FILE *fp = fopen(argv[1], "rb");
	world w;
	try {
		w.init(fp);
	} catch (int err) {
		printf("ERROR! %d\n", err);
		return -1;
	}
	fclose(fp);

	int x, y, z;
	for (y=56; y<=57; ++y) {
		printf("layer %d:\n", y);
		puts("z\\x 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
		for (z=0; z<16; ++z) {
			printf("%2X ", z & 15);
			for (x=0; x<16; ++x) {
				printf("%02X ", w.getXYZ(x, y, z));
			}
			puts("");
		}
	}
	for (y=56; y<=59; ++y) {
		for (z=0; z<16; ++z) {
			for (x=0; x<16; ++x) {
				vector<int> res;
				analyze(w, x, y, z, res);
				if (!res.empty() && res.front() >= 0) {
					printf("block %s at (%d, %d, %d) from", block_to_string(w.getXYZ(x, y, z)).c_str(), x, y, z);
					for (const int &coord : res) {
						if (coord >= 0) printf(" (%d, %d, %d)", coord >> 17, (coord >> 9) & 255, coord & 511);
						else printf(" /");
					}
					puts("");
				}
			}
		}
	}
	return 0;
}
