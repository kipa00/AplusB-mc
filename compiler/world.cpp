#include "world.hpp"

world::world() {
	this->world_data = nullptr;
}

void world::init(FILE *fp) {
	int *arr = new int[1024];
	byte *temp = new byte[1048576];
	byte *inflated = new byte[1048576];
	int i;

	fseek(fp, 0, SEEK_SET);
	fread(temp, 1, 4096, fp);

	for (i=0; i<1024; ++i) {
		arr[i] = (temp[i << 2] << 16) | (temp[(i << 2) | 1] << 8) | temp[(i << 2) | 2];
	}

	// for performance issues
	for (i=0; i<1024; ++i) {
		if (arr[i]) {
			fseek(fp, arr[i] << 12, SEEK_SET);
			fread(temp, 1, 5, fp);
			int sz = (temp[0] << 24) | (temp[1] << 16) | (temp[2] << 8) | temp[3];
			if (temp[4] != 2) {
				throw UNSUPPORTED_COMPRESSION_ERROR;
			}
			fread(temp, 1, sz - 1, fp);

			{
				z_stream infstream;
				infstream.zalloc = Z_NULL;
				infstream.zfree = Z_NULL;
				infstream.opaque = Z_NULL;

				infstream.avail_in = (uInt)(sz - 1);
				infstream.next_in = (Bytef *)temp;
				infstream.avail_out = (uInt)1048576;
				infstream.next_out = (Bytef *)inflated;

				inflateInit(&infstream);
				if (inflate(&infstream, Z_NO_FLUSH) != Z_STREAM_END) {
					throw TOO_BIG_SECTION_ERROR;
				}
				inflateEnd(&infstream);
			}

			if (!this->world_data) {
				this->world_data = new chunk *[1024];
				memset(this->world_data, 0, sizeof(chunk *) * 1024);
			}
			this->world_data[i] = new chunk();
			try {
				this->world_data[i]->read(inflated);
			} catch (int reason) {
				if (reason == ATTR_VALUE_MISMATCH_ERROR) {
					fprintf(stderr, "Error: attribute does not have possible value; did you edit the world using hex editor?\n");
				}
				if (!this->world_data[i]->troubleshoot(i & 31, i >> 5)) {
					fprintf(stderr, "in chunk (%d, %d), exact position unknown.\n", i & 31, i >> 5);
				}
				throw reason;
			}
		}
	}
	delete []temp;
	delete []inflated;
	delete []arr;
}

byte world::getXYZ(int x, int y, int z) const {
	if (!(0 <= x && x < 512 && 0 <= y && y < 255 && 0 <= z && z < 512)) return AIR;
	if (!this->world_data) return AIR;
	int idx = ((x >> 4) & 31) | (((z >> 4) & 31) << 5);
	if (!this->world_data[idx]) return AIR;
	return this->world_data[idx]->getXYZ(x, y, z);
}

world::~world() {
	if (this->world_data) {
		int i;
		for (i=0; i<1024; ++i) {
			if (this->world_data[i]) {
				delete this->world_data[i];
			}
		}
		delete []this->world_data;
		this->world_data = nullptr;
	}
}

bool world::has_world_data(int cx, int cy, int cz) const {
	if (!(0 <= cx && cx < 32 && 0 <= cy && cy < 16 && 0 <= cz && cz < 32)) return false;
	if (!this->world_data) return false;
	if (!this->world_data[(cz << 5) | cx]) return false;
	return this->world_data[(cz << 5) | cx]->has_world_data(cy);
}
