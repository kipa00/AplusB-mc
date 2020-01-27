#include "chunk.hpp"

int need_progress(int stage, string name) {
	if (stage == 0 && name == "") return 1;
	else if (stage == 1 && name == "Level") return 2;
	else if (stage == 2 && name == "Sections") return 3;
	else if (stage == 3 && name == "Palette") return 4;
	else if (stage == 4 && name == "Properties") return 5;
	return stage;
}

void chunk::feed(int y) {
	printf("Y : %d\n", y);
}

void chunk::feed(string name) {
	printf("  Block : %s\n", name.c_str());
}

void chunk::feed(string attr, string val) {
	printf("    %s : %s\n", attr.c_str(), val.c_str());
}

void chunk::feed(byte *data, int len) {
	printf("world data read\n");
}

void chunk::flush_section() {
	printf("section end\n");
}

void chunk::flush_block() {
	printf("  attr end\n");
}

bool chunk::read(byte *data, int *pos, int tag_type, int stage) {
	bool named;
	fflush(stdout);
	string name;
	if (tag_type < 0) {
		tag_type = data[(*pos)++];
		if (!tag_type) {
			return false;
		}

		int len = data[(*pos)++];
		(len <<= 8) |= data[(*pos)++];
		name = bytes_to_string(data+*pos, len);
		named = true;
		*pos += len;
	} else {
		named = false;
	}
	switch (tag_type) {
		case 1:
			if (stage == 3 && named && name == "Y") {
				this->feed((int)data[*pos]);
			}
			++*pos;
			break;
		case 2:
			*pos += 2;
			break;
		case 3:
			if (stage == 2 && named && (name == "xPos" || name == "zPos")) {
				u32 coord = 0;
				for (int i=0; i<4; ++i) {
					(coord <<= 8) |= data[(*pos)++];
				}
				(name[0] == 'x' ? this->x : this->z) = (int)coord;
				printf("%c : %d\n", name[0], (int)coord);
			} else *pos += 4;
			break;
		case 4:
			*pos += 8;
			break;
		case 5:
			*pos += 4;
			break;
		case 6:
			*pos += 8;
			break;
		case 7:
			{
				int len = 0;
				for (int i=0; i<4; ++i) {
					(len <<= 8) |= data[(*pos)++];
				}
				*pos += len;
			}
			break;
		case 8:
			{
				int len = data[(*pos)++];
				(len <<= 8) |= data[(*pos)++];
				if (stage == 4 && named && name == "Name") this->feed(bytes_to_string(data+*pos, len));
				if (stage == 5 && named) this->feed(name, bytes_to_string(data+*pos, len));
				*pos += len;
			}
			break;
		case 9:
			{
				byte subid = data[(*pos)++];
				int len = 0;
				for (int i=0; i<4; ++i) {
					(len <<= 8) |= data[(*pos)++];
				}
				int next_stage = named ? need_progress(stage, name) : stage;
				while (len--) {
					this->read(data, pos, (int)subid, next_stage);
					if (stage == 2 && next_stage == 3) this->flush_section();
					if (stage == 3 && next_stage == 4) this->flush_block();
				}
			}
			break;
		case 10:
			{
				int next_stage = named ? need_progress(stage, name) : stage;
				while (this->read(data, pos, -1, next_stage));
			}
			break;
		case 11:
			{
				int len = 0;
				for (int i=0; i<4; ++i) {
					(len <<= 8) |= data[(*pos)++];
				}
				*pos += len * 4;
			}
			break;
		case 12:
			{
				int len = 0;
				for (int i=0; i<4; ++i) {
					(len <<= 8) |= data[(*pos)++];
				}
				if (stage == 3 && named && name == "BlockStates") {
					this->feed(data+*pos, len * 8);
				}
				*pos += len * 8;
			}
			break;
	}
	return true;
}

void chunk::read(byte *data) {
	int pos = 0;
	this->read(data, &pos);
}
