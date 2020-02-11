#include "chunk.hpp"

//#define CHUNK_DEBUG

int need_progress(int stage, string name) {
	if (stage == 0 && name == "") return 1;
	else if (stage == 1 && name == "Level") return 2;
	else if (stage == 2 && name == "Sections") return 3;
	else if (stage == 3 && name == "Palette") return 4;
	else if (stage == 4 && name == "Properties") return 5;
	return stage;
}

void chunk::feed(int y) {
	this->y = y;
#ifdef CHUNK_DEBUG
	printf("Y : %d\n", y);
#endif
}

void chunk::feed(string name) {
	if (name == "minecraft:air") {
		(this->block &= 15) |= AIR;
	} else if (name == "minecraft:bedrock") {
		(this->block &= 15) |= BEDROCK;
	} else if (name == "minecraft:stone") {
		(this->block &= 15) |= STONE;
	} else if (name == "minecraft:sandstone") {
		(this->block &= 15) |= SANDSTONE;
	} else if (name == "minecraft:redstone_wire") {
		(this->block &= 15) |= REDSTONE_WIRE;
	} else if (name == "minecraft:redstone_torch") {
		(this->block &= 15) |= REDSTONE_TORCH;
	} else if (name == "minecraft:redstone_wall_torch") {
		(this->block &= 15) |= REDSTONE_WALL_TORCH;
	} else if (name == "minecraft:repeater") {
		(this->block &= 31) |= REPEATER;
	} else if (name == "minecraft:comparator") {
		(this->block &= 15) |= COMPARATOR;
	} else if (name == "minecraft:redstone_lamp") {
		(this->block &= 15) |= REDSTONE_LAMP;
	} else if (name == "minecraft:lever") {
		(this->block &= 15) |= LEVER;
	} else if (name == "minecraft:lime_concrete") {
		(this->block &= 15) |= CONCRETE | 0;
	} else if (name == "minecraft:light_blue_concrete") {
		(this->block &= 15) |= CONCRETE | 1;
	} else {
		throw UNKNOWN_BLOCK_ERROR;
	}
#ifdef CHUNK_DEBUG
	printf("  Block : %s\n", name.c_str());
#endif
}

void chunk::feed(string attr, string val) {
	if (attr == "facing") {
		if (val == "east") {
			this->block |= EAST;
		} else if (val == "west") {
			this->block |= WEST;
		} else if (val == "south") {
			this->block |= SOUTH;
		} else if (val == "north") {
			this->block |= NORTH;
		}
	} else if (attr == "power") {
		int power;
		if (sscanf(val.c_str(), "%d", &power) != 1 || !(0 <= power && power <= 15)) {
			throw REDSTONE_POWER_ERROR;
		}
		(this->block &= 240) |= power;
	} else if (attr == "lit" || attr == "powered") {
		if (val == "true") {
			this->block |= POWERED;
		} else if (val != "false") {
			throw REDSTONE_NONDUST_POWERED_ERROR;
		}
	} else if (attr == "delay") {
		int delay;
		if (sscanf(val.c_str(), "%d", &delay) != 1 || !(1 <= delay && delay <= 4)) {
			throw REDSTONE_REPEATER_DELAY_ERROR;
		}
		this->block |= (delay - 1) << 3;
	} else if (attr == "mode") {
		if (val == "subtract") {
			this->block |= SUBTRACT;
		} else if (val != "compare") {
			throw REDSTONE_COMPARATOR_MODE_ERROR;
		}
	}
#ifdef CHUNK_DEBUG
	printf("    %s : %s\n", attr.c_str(), val.c_str());
#endif
}

void chunk::init() {
	this->block = 0;
	this->y = 255;
	this->palette.clear();
	this->world_data = new byte *[16];
	memset(this->world_data, 0, sizeof(byte *) * 16);
}

void chunk::feed(const byte *data, int len) {
	this->br.init(data);
#ifdef CHUNK_DEBUG
	printf("world data read\n");
#endif
}

void chunk::flush_section() {
	if (0 <= this->y && this->y < 16) {
		if (!this->br.init_needed()) {
			if (this->world_data[this->y]) {
				throw DUPLICATE_Y_SECTION_ERROR;
			}
			this->world_data[this->y] = new byte[4096];
			const int needed_bit = this->palette.size() < 2U ? 4 : std::max(4, 32 - __builtin_clz((int)this->palette.size() - 1));
			for (int i=0; i<4096; ++i) {
				int x = this->br.read(needed_bit);
				this->world_data[this->y][i] = this->palette[x];
			}
		}
	}
	this->block = 0;
	this->y = 255;
	this->palette.clear();
	this->br.init(nullptr);
#ifdef CHUNK_DEBUG
	printf("section end\n");
#endif
}

void chunk::flush_block() {
	this->palette.push_back(this->block);
	this->block = 0;
#ifdef CHUNK_DEBUG
	printf("block = %02X\n", this->block);
#endif
}

bool chunk::read(const byte *data, int *pos, int tag_type, int stage) {
	bool named;
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

void chunk::read(const byte *data) {
	int pos = 0;
	this->init();
	this->read(data, &pos);
}

chunk::~chunk() {
	if (this->world_data) {
		int i;
		for (i=0; i<16; ++i) {
			if (this->world_data[i]) {
				delete [](this->world_data[i]);
			}
		}
		delete []this->world_data;
		this->world_data = nullptr;
	}
}

byte chunk::getXYZ(int x, int y, int z) {
	if (!this->world_data) return AIR;
	int idx = y >> 4;
	if (!this->world_data[idx]) return AIR;
	return this->world_data[idx][((y & 15) << 8) | ((z & 15) << 4) | (x & 15)];
}

bool chunk::has_world_data(int cy) const {
	if (!(0 <= cy && cy < 16)) return false;
	if (!this->world_data) return false;
	if (!this->world_data[cy]) return false;
	return true;
}
