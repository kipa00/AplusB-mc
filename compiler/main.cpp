#include <cstdio>
#include "world.hpp"
#include "analyze.hpp"
#include "check.hpp"

const int BUFSIZE = 1048576;
byte data[BUFSIZE];

void traverse_world(const world &w, std::function<void(int, int, int)> f) {
	int cx, cy, cz;
	for (cz=0; cz<32; ++cz) {
		for (cx=0; cx<32; ++cx) {
			for (cy=0; cy<16; ++cy) {
				if (w.has_world_data(cx, cy, cz)) {
					int x, y, z;
					for (z=0; z<16; ++z) {
						for (x=0; x<16; ++x) {
							for (y=0; y<16; ++y) {
								f((cx << 4) | x, (cy << 4) | y, (cz << 4) | z);
							}
						}
					}
				}
			}
		}
	}
}

void write_byte(FILE *fp, byte c) {
	fwrite(&c, 1, 1, fp);
}

void write_int(FILE *fp, u32 x) {
	write_byte(fp, x >> 24);
	write_byte(fp, (x >> 16) & 255);
	write_byte(fp, (x >> 8) & 255);
	write_byte(fp, x & 255);
}

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

	int **u = new int *[16384], cnt = 0;
	memset(u, 0, sizeof(int *) * 16384);
	traverse_world(w, [&] (int x, int y, int z) {
		if (is_redstone_component(w, x, y, z)) {
			int **ptr = u + (((z >> 4) << 9) | ((x >> 4) << 4) | (y >> 4));
			if (!*ptr) {
				*ptr = new int[4096];
				memset(*ptr, 0, sizeof(int) * 4096);
			}
			(*ptr)[((z & 15) << 8) | ((x & 15) << 4) | (y & 15)] = ++cnt;
			printf("#%d at (%d, %d, %d)\n", cnt, x, y, z);
		}
	});

	fp = fopen("a.out", "wb");
	traverse_world(w, [&] (int x, int y, int z) {
		vector<int> info;
		const int type = analyze(w, x, y, z, info);
		if (type) {
			int byte_type = 0;
			if (type == 1) {
				byte_type = 16 | (w.getXYZ(x, y, z) & 15);
			} else {
				byte_type = (type << 4) | (w.getXYZ(x, y, z) & POWERED ? 15 : 0);
			}
			write_byte(fp, byte_type);
			for (const int &x : info) {
				write_int(fp, x >= 0 ? u[x >> 12][x & 4095] : -1);
			}
			write_int(fp, -1);
			if (type == 1 || type == 2) {
				for (int i=0; i<wide_len; ++i) {
					const int nx = x + wide_dx[i], ny = y + wide_dy[i], nz = z + wide_dz[i];
					if (is_redstone_component(w, nx, ny, nz)) {
						int idx = pack(nx, ny, nz);
						write_int(fp, u[idx >> 12][idx & 4095]);
					}
				}
			} else {
				for (int i=0; i<narrow_len; ++i) {
					int nx = x + narrow_dx[i], ny = y + narrow_dy[i], nz = z + narrow_dz[i];
					if (is_redstone_component(w, nx, ny, nz)) {
						int idx = pack(nx, ny, nz);
						write_int(fp, u[idx >> 12][idx & 4095]);
					}
					if (type != 6) {
						switch (w.getXYZ(x, y, z) & 3) {
							case EAST: --nx; break;
							case WEST: ++nx; break;
							case SOUTH: --nz; break;
							case NORTH: ++nz; break;
						}
						if (is_redstone_component(w, nx, ny, nz) && !(nx == x && nz == z)) {
							int idx = pack(nx, ny, nz);
							write_int(fp, u[idx >> 12][idx & 4095]);
						}
					}
				}
			}
			write_int(fp, -1);
		}
	});
	write_byte(fp, -1);

	vector<std::pair<int, int> > inputs[16], output;
	traverse_world(w, [&] (int x, int y, int z) {
		byte fetched = w.getXYZ(x, y, z);
		if ((fetched & BLOCK_ONLY) == LEVER) {
			int nx = x, ny = y, nz = z;
			switch (fetched & 3) {
				case EAST: --nx; break;
				case WEST: ++nx; break;
				case SOUTH: --nz; break;
				case NORTH: ++nz; break;
			}
			byte facing = w.getXYZ(nx, ny, nz);
			if ((facing & BLOCK_ONLY) == CONCRETE) {
				inputs[facing & 15].emplace_back((z << 17) | (x << 8) | y, pack(x, y, z));
			}
		} else if ((fetched & BLOCK_ONLY) == REDSTONE_LAMP) {
			output.emplace_back((z << 17) | (x << 8) | y, pack(x, y, z));
		}
	});
	for (int i=0; i<16; ++i) {
		sort(inputs[i].begin(), inputs[i].end());
	}
	sort(output.begin(), output.end());

	// TODO: temporary input order, change when command parser is implemented
	for (int i=0; i<2; ++i) {
		for (const auto &[_, idx] : inputs[i]) {
			write_int(fp, u[idx >> 12][idx & 4095]);
		}
		write_int(fp, -1);
	}
	write_int(fp, -1);
	for (const auto &[_, idx] : output) {
		write_int(fp, u[idx >> 12][idx & 4095]);
	}
	write_int(fp, -1);
	fclose(fp);
	return 0;
}
